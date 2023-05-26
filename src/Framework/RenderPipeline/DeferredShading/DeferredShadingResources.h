#pragma once
#include <vector>
#include "../RenderPipelineResources.h"
#include "../ParallelLightShadowMap.h"
#include "../../RenderTexture/RenderTexture.h"
#include "../Renderer.h"

namespace Framework 
{
	namespace DeferredShading 
	{
		class DeferredRenderPipeline;
		class DeferredShadingResources : public RenderPiplineResources 
		{
			friend DeferredRenderPipeline;
		public:
			DeferredShadingResources();
			~DeferredShadingResources() override;
			//
			ParallelLightShadowMap* parallelShadowMap();
			RenderTexture* GetCameraDepthTexture();
			RenderTexture* GBuffer(UINT index);
			RendererVector* GetVisibleOpaqueRenderes();
			RendererVector* GetVisibleTransparentRenderers();
			UINT GetGBufferCnt();
		private:
			ParallelLightShadowMap* m_parallelSM;
			RenderTexture* m_cameraDepthRT;
			std::vector<RenderTexture*> m_GBuffers;
			RendererVector m_visibleOpaqueRenderers;
			RendererVector m_visibleTranparentRenderers;
		};
	}
}