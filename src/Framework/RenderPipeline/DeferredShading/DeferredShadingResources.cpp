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
			int winWidth = win_GetWidth();
			int winHeight = win_GetHeight();
			UINT msaaSamCount = d3dGraphic::GetMsaaSampleCount();
			RenderTextureDesc rtDesc = { 0 };
			rtDesc.width = winWidth;
			rtDesc.height = winHeight;
			rtDesc.includeColor = false;
			rtDesc.includeDepthStencil = true;
			rtDesc.msaaCount = msaaSamCount;
			//
			m_cameraDepthRT = RenderTextureManager::CreateRenderTexture(&rtDesc);
			//
			rtDesc.includeColor = true;
			rtDesc.includeDepthStencil = false;
			for (int g = 0; g < 4; ++g) 
			{
				RenderTexture* gbuffer = RenderTextureManager::CreateRenderTexture(&rtDesc);
				m_GBuffers.push_back(gbuffer);
			}
		}

		DeferredShadingResources::~DeferredShadingResources() 
		{
			RenderTextureManager::ReleaseRenderTexture(m_cameraDepthRT);
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
			RenderPiplineResources::~RenderPiplineResources();
		}

		ParallelLightShadowMap* DeferredShadingResources::parallelShadowMap()
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

		RendererVector* DeferredShadingResources::GetVisibleOpaqueRenderes() 
		{
			return &m_visibleOpaqueRenderers;
		}

		RendererVector* DeferredShadingResources::GetVisibleTransparentRenderers()
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
	}
}