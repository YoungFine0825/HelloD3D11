#include <algorithm>
#include "../Graphic.h"
#include "../Material/Material.h"
#include "../Shader/Shader.h"
#include "../Scene/Camera.h"
#include "../Scene/Light.h"
#include "../Scene/Entity.h"
#include "../Collision/CollisionUtils.h"
#include "FrameData.h"
#include "ForwardRenderPipeline.h"

namespace Framework 
{
	ForwardRenderPipeline::ForwardRenderPipeline() :RenderPipeline() 
	{

	}

	ForwardRenderPipeline::~ForwardRenderPipeline() 
	{
		for (size_t i = 0; i < m_lightLists.size(); ++i) 
		{
			delete m_lightLists[i];
		}
		m_lightLists.clear();
		m_visibleRenderers.clear();
		RenderPipeline::~RenderPipeline();
	}

	void ForwardRenderPipeline::OnRender() 
	{
		size_t cameraCnt = m_frameData->cameras.size();
		for (size_t c = 0; c < cameraCnt; ++c) 
		{
			m_visibleRenderers.clear();
			//
			RenderCamera(m_frameData->cameras[c]);
			//
			if (m_drawGizmosCallBack)
			{
				m_drawGizmosCallBack(m_frameData->cameras[c]);
			}
			//
			if (m_drawGUICallBack)
			{
				m_drawGUICallBack(m_frameData->cameras[c]);
			}
			//
			m_visibleRenderers.clear();
		}
		//
		//输出到屏幕
		Graphics::Present();
	}

	void ForwardRenderPipeline::RenderCamera(Camera* camera) 
	{
		CollectVisibleRenderers(camera,&m_visibleRenderers);
		size_t rendererCnt = m_visibleRenderers.size();
		if (rendererCnt <= 0)
		{
			return;
		}
		//
		SetupCamera(camera);
		//
		SortVisibleRenderers(camera, &m_visibleRenderers);
		//
		GenLightList();
		//
		XMMATRIX viewMatrix = camera->GetViewMatrix();
		XMMATRIX projMatrix = camera->GetProjectMatrix();
		XMFLOAT3 cameraPosW = XMFloat3MultiMatrix({ 0,0,0 }, XMMatrixInverse(viewMatrix));
		//
		ShaderStruct::ParallelLight mainParallelLit;
		//传递给GPU的光照数据只能用数组，本来想用vector，但发现不行，数据传递不过去。
		ShaderStruct::PointLight* pointLights = new ShaderStruct::PointLight[m_pointLightCnt]();
		ShaderStruct::SpotLight* spotLights = new ShaderStruct::SpotLight[m_spotLightCnt]();
		//
		for (size_t r = 0; r < rendererCnt; ++r)
		{
			Renderer* renderer = m_visibleRenderers[r];
			Entity* ent = renderer->GetEntity();
			Material* mat = renderer->material;
			Shader* sh = mat->GetShader();
			//设置雾效参数
			if (m_frameData->sceneSetting.enableFog)
			{
				sh->SetEnabledTechnique("UseLinearFog");
				sh->SetVector4("g_linearFogColor", m_frameData->sceneSetting.linearFogColor);
				sh->SetFloat("g_linearFogStart", m_frameData->sceneSetting.linearFogStart);
				sh->SetFloat("g_linearFogRange", m_frameData->sceneSetting.linearFogRange);
			}
			else
			{
				sh->SetEnabledTechnique("Default");
			}
			//
			sh->SetVector3("g_CameraPosW", cameraPosW);
			//设置光照参数
			InteractedLightSet* lightSet = m_lightLists[r];
			if (lightSet != nullptr)
			{
				sh->SetStruct("g_ParallelLight", &m_curParallelLight, sizeof(m_curParallelLight));
				//
				size_t litCnt = lightSet->pointLights.size();
				for (size_t i = 0; i < m_pointLightCnt; ++i)
				{
					if (i < litCnt)
					{
						pointLights[i] = lightSet->pointLights[i];
					}
					else
					{
						pointLights[i].intensity = 0;
					}
				}
				litCnt = lightSet->spotLights.size();
				for (size_t i = 0; i < m_spotLightCnt; ++i)
				{
					if (i < litCnt)
					{
						spotLights[i] = lightSet->spotLights[i];
					}
					else
					{
						spotLights[i].intensity = 0;
					}
				}
			}
			else
			{
				sh->SetStruct("g_ParallelLight", &(mainParallelLit), sizeof(mainParallelLit));
				for (size_t i = 0; i < m_pointLightCnt; ++i)
				{
					pointLights[i].intensity = 0;
				}
				for (size_t i = 0; i < m_spotLightCnt; ++i)
				{
					spotLights[i].intensity = 0;
				}
			}
			sh->SetStruct("g_PointLights", pointLights, sizeof(ShaderStruct::PointLight) * m_pointLightCnt);
			sh->SetStruct("g_SpotLights", spotLights, sizeof(ShaderStruct::SpotLight) * m_spotLightCnt);
			//设置Tranform参数
			Transform* trans = ent->GetTransform();
			XMMATRIX worldMat = trans->GetWorldMatrix();
			sh->SetMatrix4x4("obj_MatWorld", worldMat);
			XMMATRIX normalWorldMat = XMMatrixInverseTranspose(worldMat);
			sh->SetMatrix4x4("obj_MatNormalWorld", normalWorldMat);
			XMMATRIX mvp = worldMat * viewMatrix * projMatrix;
			sh->SetMatrix4x4("obj_MatMVP", mvp);
			//
			mat->Apply();
			//绘制
			Graphics::DrawMesh(renderer->mesh, sh);
		}
		//
		delete[] pointLights;
		delete[] spotLights;
	}

	void ForwardRenderPipeline::GenLightList()
	{
		//
		size_t litCnt = m_frameData->lights.size();
		//找出强度最大的平行光
		ShaderStruct::ParallelLight maxInstensityParallelLit;
		for (size_t i = 0; i < litCnt; ++i)
		{
			Light* lit = m_frameData->lights[i];
			if (lit->GetType() != LIGHT_TYPE_DIRECTIONAL) { continue; }
			if (lit->GetIntensity() > maxInstensityParallelLit.intensity)
			{
				maxInstensityParallelLit.intensity = lit->GetIntensity();
				maxInstensityParallelLit.color = lit->GetColor();
				maxInstensityParallelLit.directionW = lit->GetTransform()->GetWorldSpaceForward();
			}
		}
		//
		size_t rendererCnt = m_visibleRenderers.size();
		for (size_t r = 0; r < rendererCnt; ++r) 
		{
			InteractedLightSet* interactedLights;
			if (r >= m_lightLists.size()) 
			{
				interactedLights = new InteractedLightSet();
				m_lightLists.push_back(interactedLights);
			}
			else 
			{
				interactedLights = m_lightLists[r];
				interactedLights->pointLights.clear();
				interactedLights->spotLights.clear();
			}
			//
			Renderer* renderer = m_visibleRenderers[r];
			AxisAlignedBox aabb;
			CollisionUtils::ComputeRendererWorldSpaceAxisAlignedBox(&aabb, renderer);
			//判断该物体与哪些点光源、聚光灯相交
			for (size_t litIndex = 1; litIndex < litCnt; ++litIndex)
			{
				Light* lit = m_frameData->lights[litIndex];
				LIGHT_TYPE type = lit->GetType();
				Transform* litTransform = lit->GetTransform();
				if (type == LIGHT_TYPE_POINT) 
				{
					Sphere sphere;
					sphere.Center = litTransform->position;
					sphere.Radius = lit->GetRange();
					if (XNA::IntersectSphereAxisAlignedBox(&sphere, &aabb) > 0) 
					{
						ShaderStruct::PointLight pointLit;
						pointLit.color = lit->GetColor();
						pointLit.intensity = lit->GetIntensity();
						pointLit.attenuation = lit->GetAttenuation();
						pointLit.positionW = litTransform->position;
						pointLit.range = lit->GetRange();
						interactedLights->pointLights.push_back(pointLit);
					}
				}
				else if (type == LIGHT_TYPE_SPOT) 
				{
					//先忽略位移和旋转，构建一个轴对齐(左手坐标系)的OBB
					float range = lit->GetRange();//
					float radius = tan(Angle2Radin(lit->GetSpot())) * range;
					XMFLOAT3 forwardW = litTransform->GetWorldSpaceForward();
					XMFLOAT3 rotationW = litTransform->rotation;
					XMFLOAT3 positionW = litTransform->position;
					OBB obbAA;
					obbAA.Center = { 0,0,range / 2 };
					XMVECTOR noneRotation = XMQuaternionRotationRollPitchYaw(0, 0, 0);
					XMStoreFloat4(&obbAA.Orientation, noneRotation);
					XMFLOAT3 endPoint = { 0,0,range };
					XMFLOAT3 right = { 1,0,0 };
					XMFLOAT3 up = { 0,1,0 };
					XMFLOAT3 max = endPoint + right * radius + up * radius;
					obbAA.Extents = max - obbAA.Center;
					//然后应用位移和旋转
					OBB obb;
					obb.Center = positionW + forwardW * (range / 2);
					XMMATRIX rotationMatrix = XMMatrixRotationFromFloat3(rotationW);
					obb.Extents = XMFloat3MultiMatrix(obbAA.Extents, rotationMatrix);
					XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(rotationW.x, rotationW.y, rotationW.z);
					XMStoreFloat4(&obb.Orientation, rotation);
					//XNA::TransformOrientedBox(
					//	&obb, 
					//	&obbAA,
					//	1,
					//	XMQuaternionRotationRollPitchYaw(rotationW.x, rotationW.y, rotationW.z),
					//	XMVectorSet({ positionW.x,positionW.y,positionW.z,1.0 })
					//);
					if (XNA::IntersectAxisAlignedBoxOrientedBox(&aabb, &obb) > 0) 
					{
						ShaderStruct::SpotLight spotLit;
						spotLit.intensity = lit->GetIntensity();
						spotLit.color = lit->GetColor();
						spotLit.positionW = positionW;
						spotLit.directionW = litTransform->GetWorldSpaceForward();
						spotLit.attenuation = lit->GetAttenuation();
						spotLit.range = lit->GetRange();
						spotLit.spot = Angle2Radin(lit->GetSpot());
						interactedLights->spotLights.push_back(spotLit);
					}
				}
			}
			//
			m_curParallelLight = maxInstensityParallelLit;
		}
	}
}