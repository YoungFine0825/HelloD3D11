#pragma once
#include "DeferredShadingPass.h"

namespace Framework 
{
	namespace DeferredShading 
	{
		class ParallelShadowPass : public DeferredShadingPass 
		{
		public:
			ParallelShadowPass();
			~ParallelShadowPass() override;
			//
			void Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps) override;
		protected:
			void OnFrameStart() override;
			void OnInvoke() override;
		private:

		};
	}
}