#pragma once
#include "DeferredShadingPass.h"

namespace Framework
{
	namespace DeferredShading
	{
		class BackgroundPass : public DeferredShadingPass
		{
		public:
			BackgroundPass();
			~BackgroundPass() override;
			//
			void Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps) override;
		protected:
			void OnFrameStart() override;
			void OnInvoke() override;
		private:

		};
	}
}