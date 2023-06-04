#include "ShadowPass.h"
#include "DeferredRenderPipeline.h"
#include "DeferredShadingResources.h"
#include "../ParallelLightShadowMap.h"

namespace Framework 
{
	namespace DeferredShading 
	{
		ShadowPass::ShadowPass() 
		{

		}

		ShadowPass::~ShadowPass() 
		{
			DeferredShadingPass::~DeferredShadingPass();
		}

		void ShadowPass::Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps)
		{
			DeferredShadingPass::Init(rp, rps);
		}

		void ShadowPass::OnFrameStart() 
		{
			m_resouces->GetParallelShadowMap()->PreRender(m_renderPipeline->GetFrameData());
		}

		void ShadowPass::OnInvoke() 
		{
			Light* parallelLit = m_resouces->GetParallelLight();
			if (!parallelLit) 
			{
				m_resouces->GetParallelShadowMap()->Clear();
				return;
			}
			m_resouces->GetParallelShadowMap()->Update(
				m_resouces->GetRenderingCamera(),
				m_resouces->GetOpaqueRenderes(),
				&m_renderPipeline->GetFrameData()->renderers,
				parallelLit->GetTransform()->rotation
			);
		}
	}
}