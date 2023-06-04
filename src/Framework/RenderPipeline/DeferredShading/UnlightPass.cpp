#include "DeferredRenderPipeline.h"
#include "DeferredShadingResources.h"
#include "UnlightPass.h"

namespace Framework 
{
	namespace DeferredShading 
	{
		UnlightPass::UnlightPass() : DeferredShadingPass() 
		{

		}

		UnlightPass::~UnlightPass() 
		{
			DeferredShadingPass::~DeferredShadingPass();
		}

		void UnlightPass::Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps) 
		{
			DeferredShadingPass::Init(rp, rps);
		}

		void UnlightPass::OnFrameStart() 
		{

		}

		void UnlightPass::OnInvoke() 
		{
			RenderTexture* resultRT = m_resouces->GetShadingResultTexture();
			RenderingCameraInfo* cameraInfo = m_resouces->GetRenderingCameraInfo();
			RendererVector* renderers = m_resouces->GetUnlightOpaqueRenderers();
			size_t rendererCnt = renderers->size();
			//
			Graphics::SetRenderTarget(resultRT);
			Graphics::SetDepthStencil(m_resouces->GetCameraDepthTexture());
			//
			for (size_t r = 0; r < rendererCnt; ++r)
			{
				Renderer* renderer = (*renderers)[r];
				Material* materialInst = renderer->GetMaterialInstance();
				Shader* shader = materialInst->GetShader();
				m_renderPipeline->DrawRenderer(renderer, shader, cameraInfo->viewMatrix, cameraInfo->projMatrix);
			}
		}
	}
}