#include <algorithm>
#include "../../App.h"
#include "../Graphic.h"
#include "../Material/Material.h"
#include "../Shader/Shader.h"
#include "../Scene/Camera.h"
#include "../Scene/Light.h"
#include "../Scene/Entity.h"
#include "../Collision/CollisionUtils.h"
#include "../Shader/ShaderManager.h"
#include "../Shader/Shader.h"
#include "FrameData.h"
#include "ForwardRenderPipeline.h"
#include "ParallelLightShadowMap.h"

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
		if (m_shadowMap) 
		{
			delete m_shadowMap;
			m_shadowMap = nullptr;
		}
		RenderPipeline::~RenderPipeline();
	}

	ParallelLightShadowMap* ForwardRenderPipeline::GetParallelLightShadowMap() 
	{
		if (!m_shadowMap)
		{
			m_shadowMap = new ParallelLightShadowMap();
		}
		return m_shadowMap;
	}

	void ForwardRenderPipeline::OnRender() 
	{
		size_t cameraCnt = m_frameData->cameras.size();
		//
		FindMaxIntensityParallelLight();
		//
		if (m_shadowMap) 
		{
			m_shadowMap->PreRender(m_frameData);
		}
		//Draw Scene
		for (size_t c = 0; c < cameraCnt; ++c) 
		{
			//
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
		if (m_shadowMap)
		{
			m_shadowMap->PostRender();
		}
		//
		//输出到屏幕
		Graphics::Present();
	}

	void ForwardRenderPipeline::RenderCamera(Camera* camera) 
	{
		//
		SetupCamera(camera);
		//
		CollectVisibleRenderers(camera,&m_visibleRenderers);
		size_t rendererCnt = m_visibleRenderers.size();
		if (rendererCnt <= 0)
		{
			return;
		}
		//
		SortVisibleRenderers(camera, &m_visibleRenderers);
		//Draw ShadowMap
		if (m_shadowMap && m_maxIntensityParallelLight.intensity > 0)
		{
			m_shadowMap->Update(camera,&m_visibleRenderers, &m_frameData->renderers, m_maxIntensityParallelLightRotW);
		}
		//
		GenLightList();
		//
		XMMATRIX viewMatrix = camera->GetViewMatrix();
		XMMATRIX projMatrix = camera->GetProjectMatrix();
		XMFLOAT3 cameraPosW = camera->GetTransform()->position;
		//
		ShaderStruct::ParallelLight mainParallelLit;
		//传递给GPU的光照数据用数组。本来想用vector，但发现不行，数据传递不过去。
		ShaderStruct::PointLight* pointLights = new ShaderStruct::PointLight[m_pointLightCnt]();
		ShaderStruct::SpotLight* spotLights = new ShaderStruct::SpotLight[m_spotLightCnt]();
		//
		for (size_t r = 0; r < rendererCnt; ++r)
		{
			Renderer* renderer = m_visibleRenderers[r];
			Entity* ent = renderer->GetEntity();
			Material* mat = renderer->GetMaterialInstance();
			Shader* sh = mat->GetShader();
			//Per Frame
			sh->SetFloat("g_timeDelta", App_GetTimeDelta());
			sh->SetVector3("g_CameraPosW", cameraPosW);
			//设置光照参数
			InteractedLightSet* lightSet = m_lightLists[r];
			if (lightSet != nullptr)
			{
				sh->SetStruct("g_ParallelLight", &m_maxIntensityParallelLight, sizeof(m_maxIntensityParallelLight));
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
			//设置ShadowMap参数
			bool useShadow = mat->IsReceiveShadow() && m_shadowMap != nullptr;
			if (useShadow) 
			{
				m_shadowMap->SetShaderParamters(sh);
			}
			//设置雾效参数
			if (m_frameData->sceneSetting.enableFog)
			{
				sh->SetVector4("g_linearFogColor", m_frameData->sceneSetting.linearFogColor);
				sh->SetFloat("g_linearFogStart", m_frameData->sceneSetting.linearFogStart);
				sh->SetFloat("g_linearFogRange", m_frameData->sceneSetting.linearFogRange);
			}
			//设置要使用Technique
			if (useShadow) 
			{
				if (m_frameData->sceneSetting.enableFog)
				{
					sh->SetEnabledTechnique("UseLinearFogAndShadow");
				}
				else
				{
					sh->SetEnabledTechnique("UseShadow");
				}
			}
			else 
			{
				if (m_frameData->sceneSetting.enableFog) 
				{
					sh->SetEnabledTechnique("UseLinearFog");
				}
				else 
				{
					sh->SetEnabledTechnique("Default");
				}
			}
			//设置Tranform参数
			Transform* trans = ent->GetTransform();
			XMMATRIX worldMat = trans->GetWorldMatrix();
			sh->SetMatrix4x4("obj_MatWorld", worldMat);
			XMMATRIX normalWorldMat = XMMatrixInverseTranspose(worldMat);
			sh->SetMatrix4x4("obj_MatNormalWorld", normalWorldMat);
			XMMATRIX mvp = worldMat * viewMatrix * projMatrix;
			sh->SetMatrix4x4("obj_MatMVP", mvp);
			sh->SetMatrix4x4("obj_MatView", viewMatrix);
			sh->SetMatrix4x4("obj_MatProj", projMatrix);
			//
			mat->Apply();
			//绘制
			Graphics::DrawMesh(renderer->GetMeshInstance(), sh);
		}
		//
		delete[] pointLights;
		delete[] spotLights;
	}

	void ForwardRenderPipeline::FindMaxIntensityParallelLight()
	{
		size_t litCnt = m_frameData->lights.size();
		//找出强度最大的平行光
		ShaderStruct::ParallelLight parallelLit;
		Light* maxIntensityLit = nullptr;
		for (size_t i = 0; i < litCnt; ++i)
		{
			Light* lit = m_frameData->lights[i];
			if (!lit->IsEnabled())
			{
				continue;
			}
			if (lit->GetType() != LIGHT_TYPE_DIRECTIONAL) { continue; }
			if (lit->GetIntensity() > parallelLit.intensity)
			{
				parallelLit.intensity = lit->GetIntensity();
				parallelLit.color = lit->GetColor();
				parallelLit.directionW = lit->GetTransform()->GetWorldSpaceForward();
				maxIntensityLit = lit;
			}
		}
		//
		m_maxIntensityParallelLight = parallelLit;
		//
		if (maxIntensityLit != nullptr) 
		{
			m_maxIntensityParallelLightRotW = maxIntensityLit->GetTransform()->rotation;
		}
		else 
		{
			m_maxIntensityParallelLightRotW = { 0,0,0 };
		}
	}

	//为每个可见的renderer搜集它受哪些光源影响
	void ForwardRenderPipeline::GenLightList()
	{
		size_t litCnt = m_frameData->lights.size();
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
			//求renderer的世界空间轴对齐包围盒
			AxisAlignedBox aabbWorldSpace;
			CollisionUtils::ComputeRendererWorldSpaceAxisAlignedBox(&aabbWorldSpace, renderer);
			//判断该物体与哪些点光源、聚光灯相交
			for (size_t litIndex = 0; litIndex < litCnt; ++litIndex)
			{
				Light* lit = m_frameData->lights[litIndex];
				if (!lit->IsEnabled())
				{ 
					continue;
				}
				LIGHT_TYPE type = lit->GetType();
				Transform* litTransform = lit->GetTransform();
				if (type == LIGHT_TYPE_POINT) 
				{
					Sphere sphere;
					sphere.Center = litTransform->position;
					sphere.Radius = lit->GetRange();
					if (XNA::IntersectSphereAxisAlignedBox(&sphere, &aabbWorldSpace) > 0)
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
					float range = lit->GetRange();
					float radin = Angle2Radin(lit->GetSpot());
					float radius = tan(radin) * range;
					//为SpotLight构建一个朝向包围盒（OBB）,用OBB与renderer的AABB判断两者是否发碰撞
					OBB obbL;//先构建一个局部空间OBB
					obbL.Center = { 0,0,range / 2.0f };
					XMFLOAT3 endPoint = {0,0,range};
					XMFLOAT3 max = { radius,radius,range };
					obbL.Extents = max - obbL.Center;
					obbL.Orientation = { 0,0,0,1 };
					//再应用光源的旋转和位移，将局部空间OBB转换到世界空间
					OBB obbW;
					XMVECTOR scale;
					XMVECTOR rotQuat;
					XMVECTOR translation;
					XMMATRIX litWorldMatrix = litTransform->GetWorldMatrix();
					XMMatrixDecompose(&scale, &rotQuat, &translation, litWorldMatrix);
					XNA::TransformOrientedBox(&obbW, &obbL, 1, rotQuat, translation);
					if (XNA::IntersectAxisAlignedBoxOrientedBox(&aabbWorldSpace, &obbW))
					{
						ShaderStruct::SpotLight spotLit;
						spotLit.intensity = lit->GetIntensity();
						spotLit.color = lit->GetColor();
						spotLit.positionW = litTransform->position;
						spotLit.directionW = litTransform->GetWorldSpaceForward();
						spotLit.attenuation = lit->GetAttenuation();
						spotLit.range = range;
						spotLit.spot = Angle2Radin(lit->GetSpot());
						interactedLights->spotLights.push_back(spotLit);
					}
				}
			}

		}
	}
}