#include "../../../Window.h"
#include "../../d3d/d3dGraphic.h"
#include "../../RenderTexture/RenderTextureManager.h"
#include "DeferredShadingResources.h"

namespace Framework 
{
	namespace DeferredShading 
	{
		DeferredShadingResources::DeferredShadingResources() 
		{
			InitResources();
		}

		DeferredShadingResources::~DeferredShadingResources() 
		{
			RenderTextureManager::ReleaseRenderTexture(m_cameraDepthRT);
			RenderTextureManager::ReleaseRenderTexture(m_finalShadingRT);
			//
			for (size_t g = 0; g < m_GBuffers.size(); ++g) 
			{
				RenderTextureManager::ReleaseRenderTexture(m_GBuffers[g]);
			}
			m_GBuffers.clear();
			//
			if (m_parallelSM != nullptr) 
			{
				delete m_parallelSM;
				m_parallelSM = nullptr;
			}
			m_visibleBgRenderers.clear();
			m_visibleOpaqueRenderers.clear();
			m_unlightOpaqueRenderers.clear();
			m_visibleTranparentRenderers.clear();
			m_visiblePunctualLight.clear();
			m_renderingCamera = nullptr;
			m_parallelLit = nullptr;
			RenderPiplineResources::~RenderPiplineResources();
		}

		void DeferredShadingResources::InitResources() 
		{
			int winWidth = win_GetWidth();
			int winHeight = win_GetHeight();
			RenderTextureDesc rtDesc = { 0 };
			rtDesc.width = winWidth;
			rtDesc.height = winHeight;
			rtDesc.includeColor = false;
			rtDesc.includeDepthStencil = true;
			rtDesc.msaaCount = 0;
			//
			m_cameraDepthRT = RenderTextureManager::CreateRenderTexture(&rtDesc);
			//
			rtDesc.includeColor = true;
			rtDesc.includeDepthStencil = false;
			rtDesc.colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			m_finalShadingRT = RenderTextureManager::CreateRenderTexture(&rtDesc);
			//
			rtDesc.colorFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			RenderTexture* gbuffer = RenderTextureManager::CreateRenderTexture(&rtDesc);
			m_GBuffers.push_back(gbuffer);//GBuffer0 world position
			//
			rtDesc.colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			gbuffer = RenderTextureManager::CreateRenderTexture(&rtDesc);
			m_GBuffers.push_back(gbuffer);//GBuffer1
			//
			rtDesc.colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			gbuffer = RenderTextureManager::CreateRenderTexture(&rtDesc);
			m_GBuffers.push_back(gbuffer);//GBuffer2
			//
			rtDesc.colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			gbuffer = RenderTextureManager::CreateRenderTexture(&rtDesc);
			m_GBuffers.push_back(gbuffer);//GBuffer3
			//
			m_parallelSM = new ParallelLightShadowMap();
			m_parallelSM->SetSize(2048);
		}

		RenderingCameraInfo* DeferredShadingResources::GetRenderingCameraInfo() 
		{
			return &m_renderingCameraInfo;
		}

		Camera* DeferredShadingResources::GetRenderingCamera() 
		{
			return m_renderingCamera;
		}

		Light* DeferredShadingResources::GetParallelLight()
		{
			return m_parallelLit;
		}

		ParallelLightShadowMap* DeferredShadingResources::GetParallelShadowMap()
		{
			return m_parallelSM;
		}

		RenderTexture* DeferredShadingResources::GBuffer(UINT index) 
		{
			if (index >= m_GBuffers.size()) 
			{
				return nullptr;
			}
			return m_GBuffers[index];
		}

		RendererVector* DeferredShadingResources::GetBackgroundRenderers() 
		{
			return &m_visibleBgRenderers;
		}

		RendererVector* DeferredShadingResources::GetOpaqueRenderes() 
		{
			return &m_visibleOpaqueRenderers;
		}

		RendererVector* DeferredShadingResources::GetUnlightOpaqueRenderers()
		{
			return &m_unlightOpaqueRenderers;
		}

		RendererVector* DeferredShadingResources::GetTransparentRenderers()
		{
			return &m_visibleTranparentRenderers;
		}

		UINT DeferredShadingResources::GetGBufferCnt() 
		{
			return m_GBuffers.size();
		}

		RenderTexture* DeferredShadingResources::GetCameraDepthTexture() 
		{
			return m_cameraDepthRT;
		}

		RenderTexture* DeferredShadingResources::GetShadingResultTexture() 
		{
			return m_finalShadingRT;
		}

		LightVector* DeferredShadingResources::GetVisiblePunctualLights()
		{
			return &m_visiblePunctualLight;
		}
	}
}