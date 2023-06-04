#pragma once
#include "DeferredShadingPass.h"

namespace Framework 
{
	class ParallelLightShadowMap;
	namespace DeferredShading 
	{
		class ShadowPass : public DeferredShadingPass 
		{
		public:
			ShadowPass();
			~ShadowPass() override;
			//
			void Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps) override;
		protected:
			void OnFrameStart() override;
			void OnInvoke() override;
		private:

		};
	}
}