#include "ParallelShadowPass.h"

namespace Framework 
{
	namespace DeferredShading 
	{
		ParallelShadowPass::ParallelShadowPass() 
		{

		}

		ParallelShadowPass::~ParallelShadowPass() 
		{
			DeferredShadingPass::~DeferredShadingPass();
		}

		void ParallelShadowPass::Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps)
		{
			DeferredShadingPass::Init(rp, rps);
		}

		void ParallelShadowPass::OnFrameStart() 
		{

		}

		void ParallelShadowPass::OnInvoke() 
		{
			
		}
	}
}