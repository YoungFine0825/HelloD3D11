#include "../../../App.h"
#include "../../Scene/Camera.h"
#include "../../Scene/Entity.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"
#include "../../Collision/CollisionUtils.h"
#include "../../Graphic.h"
#include "../FrameData.h"
#include "../Renderer.h"
#include "DeferredRenderPipeline.h"
#include "DeferredShadingResources.h"
//Passes
#include "DeferredShadingPass.h"
#include "ShadowPass.h"
#include "GBufferPass.h"
#include "LightingPass.h"
#include "UnlightPass.h"

namespace Framework
{
	namespace DeferredShading
	{
		DeferredRenderPipeline::DeferredRenderPipeline()
		{
			m_resources = new DeferredShadingResources();
			InitPasses();
		}

		DeferredRenderPipeline::~DeferredRenderPipeline()
		{
			if (m_resources)
			{
				delete m_resources;
				m_resources = nullptr;
			}
			RenderPipeline::~RenderPipeline();
		}

		void DeferredRenderPipeline::InitPasses()
		{
			m_shadowPass = std::make_shared<ShadowPass>();
			m_gbufferPass = std::make_shared<GBufferPass>();
			m_lightingPass = std::make_shared<LightingPass>();
			m_unlightPass = std::make_shared<UnlightPass>();
			//
			m_shadowPass->Init(this, this->m_resources);
			m_gbufferPass->Init(this, this->m_resources);
			m_lightingPass->Init(this, this->m_resources);
			m_unlightPass->Init(this, this->m_resources);
		}

		FrameData* DeferredRenderPipeline::GetFrameData() 
		{
			return m_frameData;
		}

		void DeferredRenderPipeline::OnRender()
		{
			size_t cameraCnt = m_frameData->cameras.size();
			//
			OnFrameStart();
			//Draw Camera
			for (size_t c = 0; c < cameraCnt; ++c)
			{
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
			}
			//输出到屏幕
			Graphics::Present();
		}

		void DeferredRenderPipeline::OnFrameStart() 
		{
			m_shadowPass->OnFrameStart();
			m_gbufferPass->OnFrameStart();
		}

		void DeferredRenderPipeline::RenderCamera(Camera* camera)
		{
			SetupCamera(camera);
			//
			RendererVector visibleRenderers;
			CollectVisibleRenderers(camera, &visibleRenderers);
			//
			size_t rendererCnt = visibleRenderers.size();
			if (rendererCnt <= 0)
			{
				return;
			}
			//
			SortVisibleRenderers(camera, &visibleRenderers);
			//
			m_resources->m_visibleOpaqueRenderers.clear();
			m_resources->m_unlightOpaqueRenderers.clear();
			m_resources->m_visibleTranparentRenderers.clear();
			for (size_t r = 0; r < rendererCnt; ++r) 
			{
				Renderer* renderer = visibleRenderers[r];
				if (renderer->IsTransparent())
				{
					m_resources->m_visibleTranparentRenderers.push_back(renderer);
				}
				else 
				{
					Material* materialInst = renderer->GetMaterialInstance();
					Shader* shader = materialInst->GetShader();
					if (!materialInst->IsEnableLighting() || !shader->hasTechnique("GBuffer"))
					{
						m_resources->m_unlightOpaqueRenderers.push_back(renderer);
					}
					else 
					{
						m_resources->m_visibleOpaqueRenderers.push_back(renderer);
					}
				}
			}
			//
			m_resources->m_renderingCamera = camera;
			m_resources->m_renderingCameraInfo.posW = camera->GetTransform()->position;
			m_resources->m_renderingCameraInfo.projMatrix = camera->GetProjectMatrix();
			m_resources->m_renderingCameraInfo.viewMatrix = camera->GetViewMatrix();
			//
			FindParallelLight();
			CollectVisiblePuntcualLights(camera);
			//
			static_cast<ShadowPass*>(m_shadowPass.get())->Invoke();
			//
			static_cast<GBufferPass*>(m_gbufferPass.get())->Invoke();
			//
			static_cast<LightingPass*>(m_lightingPass.get())->Invoke();
			//
			static_cast<UnlightPass*>(m_unlightPass.get())->Invoke();
			//
			Framework::RenderTexture* rt = camera->GetRenderTexture();
			Graphics::Blit(m_resources->m_finalShadingRT, rt);
		}

		void DeferredRenderPipeline::DrawRenderer(Renderer* renderer, Shader* shader, XMMATRIX viewMatrix, XMMATRIX projectMatrix,int pass)
		{
			Entity* ent = renderer->GetEntity();
			Material* mat = renderer->GetMaterialInstance();
			//Per Frame
			shader->SetFloat("g_timeDelta", App_GetTimeDelta());
			shader->SetVector3("g_CameraPosW", m_resources->m_renderingCameraInfo.posW);
			//设置Tranform参数
			Transform* trans = ent->GetTransform();
			XMMATRIX worldMat = trans->GetWorldMatrix();
			shader->SetMatrix4x4("obj_MatWorld", worldMat);
			XMMATRIX normalWorldMat = XMMatrixInverseTranspose(worldMat);
			shader->SetMatrix4x4("obj_MatNormalWorld", normalWorldMat);
			XMMATRIX mvp = worldMat * viewMatrix * projectMatrix;
			shader->SetMatrix4x4("obj_MatMVP", mvp);
			shader->SetMatrix4x4("obj_MatView", viewMatrix);
			shader->SetMatrix4x4("obj_MatProj", projectMatrix);
			//
			mat->Apply();
			//绘制
			Graphics::DrawMesh(renderer->GetMeshInstance(), shader,pass);
		}

		void DeferredRenderPipeline::FindParallelLight() 
		{
			LightVector* lights = &(m_frameData->lights);
			size_t lightCnt = lights->size();
			Light* maxIntensityParallelLit = nullptr;
			float maxIntensity = 0.0f;
			for (size_t l = 0; l < lightCnt; ++l)
			{
				if ((*lights)[l]->GetType() == LIGHT_TYPE_DIRECTIONAL && (*lights)[l]->GetIntensity() > maxIntensity)
				{
					maxIntensityParallelLit = (*lights)[l];
				}
			}
			m_resources->m_parallelLit = maxIntensityParallelLit;
		}

		void DeferredRenderPipeline::CollectVisiblePuntcualLights(Camera* camera)
		{
			m_resources->m_visiblePunctualLight.clear();
			Frustum cameraFrustum = camera->GetWorldSpaceFrustum();
			LightVector* lights = &(m_frameData->lights);
			size_t litCnt = lights->size();
			for (size_t l = 0; l < litCnt; ++l) 
			{
				Light* lit = (*lights)[l];
				LIGHT_TYPE type = lit->GetType();
				if (type == LIGHT_TYPE_DIRECTIONAL) { continue; }
				if (Framework::CollisionUtils::IntersectLightFrustum(lit, &cameraFrustum))
				{
					m_resources->m_visiblePunctualLight.push_back(lit);
				}
			}
		}
	}

}