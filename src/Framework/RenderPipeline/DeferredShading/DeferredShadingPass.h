#pragma once
#include "../RenderPass.h"

namespace Framework
{
	
	namespace DeferredShading
	{
		class DeferredRenderPipeline;
		class DeferredShadingResources;

		class DeferredShadingPass : public RenderPass
		{
		public:
			DeferredShadingPass();
			~DeferredShadingPass() override;
			//
			virtual void Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps);
			virtual void OnFrameStart();
		protected:
			void OnInvoke() override;
			DeferredRenderPipeline* m_renderPipeline;
			DeferredShadingResources* m_resouces;
		};
	}
}