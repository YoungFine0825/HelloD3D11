#pragma once
#include "DeferredShadingPass.h"

namespace Framework
{
	namespace DeferredShading
	{
		class UnlightPass : public DeferredShadingPass
		{
		public:
			UnlightPass();
			~UnlightPass() override;
			//
			void Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps) override;
		protected:
			void OnFrameStart() override;
			void OnInvoke() override;
		private:

		};
	}
}