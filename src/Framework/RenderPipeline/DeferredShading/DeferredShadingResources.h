#pragma once
#include <vector>
#include "../RenderPipelineResources.h"
#include "../ParallelLightShadowMap.h"
#include "../../RenderTexture/RenderTexture.h"
#include "../Renderer.h"

namespace Framework 
{
	class Light;
	class Camera;
	namespace DeferredShading 
	{
		class DeferredRenderPipeline;
		struct RenderingCameraInfo
		{
			XMFLOAT3 posW;
			XMMATRIX viewMatrix;
			XMMATRIX projMatrix;
		};

		class DeferredShadingResources : public RenderPiplineResources 
		{
			friend DeferredRenderPipeline;
		public:
			DeferredShadingResources();
			~DeferredShadingResources() override;
			//
			RenderingCameraInfo* GetRenderingCameraInfo();
			Camera* GetRenderingCamera();
			Light* GetParallelLight();
			ParallelLightShadowMap* GetParallelShadowMap();
			RenderTexture* GetCameraDepthTexture();
			RenderTexture* GetShadingResultTexture();
			RenderTexture* GBuffer(UINT index);
			RendererVector* GetBackgroundRenderers();
			RendererVector* GetOpaqueRenderes();
			RendererVector* GetUnlightOpaqueRenderers();
			RendererVector* GetTransparentRenderers();
			UINT GetGBufferCnt();
			LightVector* GetVisiblePunctualLights();
		private:
			void InitResources();
			RenderingCameraInfo m_renderingCameraInfo;
			Camera* m_renderingCamera;
			Light* m_parallelLit;
			ParallelLightShadowMap* m_parallelSM;
			RenderTexture* m_cameraDepthRT;
			RenderTexture* m_finalShadingRT;
			std::vector<RenderTexture*> m_GBuffers;
			RendererVector m_visibleBgRenderers;
			RendererVector m_visibleOpaqueRenderers;
			RendererVector m_unlightOpaqueRenderers;
			RendererVector m_visibleTranparentRenderers;
			LightVector m_visiblePunctualLight;
		};
	}
}