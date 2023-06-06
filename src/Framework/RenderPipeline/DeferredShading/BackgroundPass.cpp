#include "DeferredRenderPipeline.h"
#include "DeferredShadingResources.h"
#include "BackgroundPass.h"

namespace Framework
{
	namespace DeferredShading
	{
		BackgroundPass::BackgroundPass() : DeferredShadingPass()
		{

		}

		BackgroundPass::~BackgroundPass()
		{
			DeferredShadingPass::~DeferredShadingPass();
		}

		void BackgroundPass::Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps)
		{
			DeferredShadingPass::Init(rp, rps);
		}

		void BackgroundPass::OnFrameStart()
		{

		}

		void BackgroundPass::OnInvoke()
		{
			RenderTexture* resultRT = m_resouces->GetShadingResultTexture();
			RenderingCameraInfo* cameraInfo = m_resouces->GetRenderingCameraInfo();
			RendererVector* renderers = m_resouces->GetBackgroundRenderers();
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