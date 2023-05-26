#include "DeferredShadingPass.h"

namespace Framework
{
	namespace DeferredShading
	{
		DeferredShadingPass::DeferredShadingPass()
		{

		}

		DeferredShadingPass::~DeferredShadingPass()
		{
			m_renderPipeline = nullptr;
			m_resouces = nullptr;
		}

		void DeferredShadingPass::Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps) 
		{
			m_renderPipeline = rp;
			m_resouces = rps;
		}

		void DeferredShadingPass::OnInvoke() 
		{

		}

		void DeferredShadingPass::OnFrameStart()
		{

		}
	}
}