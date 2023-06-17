#include <vector>
#include "../../Collision/CollisionUtils.h"
#include "DeferredRenderPipeline.h"
#include "DeferredShadingResources.h"
#include "TransparentPass.h"

namespace Framework
{
	namespace DeferredShading
	{
		TransparentPass::TransparentPass() : DeferredShadingPass()
		{
			m_pointLightsParam = new LightParameter[m_punctualLightCnt]();
			m_spotLightsParam = new LightParameter[m_punctualLightCnt]();
		}

		TransparentPass::~TransparentPass()
		{
			delete[]  m_pointLightsParam;
			delete[] m_spotLightsParam;
			DeferredShadingPass::~DeferredShadingPass();
		}

		void TransparentPass::Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps)
		{
			DeferredShadingPass::Init(rp, rps);
		}

		void TransparentPass::OnFrameStart()
		{

		}

		void TransparentPass::OnInvoke()
		{
			RendererVector* renderers = m_resouces->GetTransparentRenderers();
			size_t rendererCnt = renderers->size();
			if (rendererCnt <= 0) 
			{
				return;
			}
			Light* parallelLit = m_resouces->GetParallelLight();
			LightVector* lights = m_resouces->GetVisiblePunctualLights();
			size_t punctualLitCnt = lights->size();
			if (!parallelLit && punctualLitCnt <= 0) 
			{
				return;
			}
			//
			if(parallelLit)
			{
				m_parallelLightParam.intensity = parallelLit->GetIntensity();
				m_parallelLightParam.color = parallelLit->GetColor();
				m_parallelLightParam.directionW = parallelLit->GetTransform()->GetWorldSpaceForward();
			}
			else 
			{
				m_parallelLightParam.intensity = 0;
			}
			//
			AxisAlignedBox aabbW;
			RenderingCameraInfo* cameraInfo = m_resouces->GetRenderingCameraInfo();
			int pointLitCnt = 0;
			int spotLitCnt = 0;
			for (size_t r = 0; r < rendererCnt; ++r)
			{
				pointLitCnt = 0;
				spotLitCnt = 0;
				Renderer* renderer = (*renderers)[r];
				CollisionUtils::ComputeRendererWorldSpaceAxisAlignedBox(&aabbW, renderer);
				for (size_t l = 0; l < punctualLitCnt; ++l)
				{
					Light* light = (*lights)[l];
					LIGHT_TYPE type = light->GetType();
					if (!CollisionUtils::IntersectLightAxisAlignedBox(light, &aabbW)) 
					{
						continue;
					}
					//
					LightParameter* litParam = nullptr;
					if (type == LIGHT_TYPE_POINT) 
					{
						litParam = &m_pointLightsParam[pointLitCnt];
						pointLitCnt++;
					}
					else if(type == LIGHT_TYPE_SPOT)
					{
						litParam = &m_spotLightsParam[spotLitCnt];
						spotLitCnt++;
					}
					//
					if (!litParam) { continue; }
					//
					litParam->intensity = light->GetIntensity();
					litParam->color = light->GetColor();
					litParam->attenuation = light->GetAttenuation();
					litParam->positionW = light->GetTransform()->position;
					litParam->range = light->GetRange();
					litParam->directionW = light->GetTransform()->GetWorldSpaceForward();
					litParam->spot = light->GetSpot();
				}
				//
				for (int pointLitIdx = pointLitCnt; pointLitIdx < m_punctualLightCnt - 1; ++pointLitIdx)
				{
					m_pointLightsParam[pointLitIdx].intensity = 0;
				}
				for (int spotLitIdx = spotLitCnt; spotLitIdx < m_punctualLightCnt - 1; ++spotLitIdx)
				{
					m_spotLightsParam[spotLitIdx].intensity = 0;
				}
				//
				Entity* ent = renderer->GetEntity();
				Material* mat = renderer->GetMaterialInstance();
				Shader* sh = mat->GetShader();
				sh->SetStruct("g_ParallelLight", &m_parallelLightParam, sizeof(m_parallelLightParam));
				sh->SetStruct("g_PointLights", m_pointLightsParam, sizeof(LightParameter) * m_punctualLightCnt);
				sh->SetStruct("g_SpotLights", m_spotLightsParam, sizeof(LightParameter) * m_punctualLightCnt);
				//
				m_renderPipeline->DrawRenderer(renderer,sh,cameraInfo->viewMatrix,cameraInfo->projMatrix);
			}
		}
	}
}