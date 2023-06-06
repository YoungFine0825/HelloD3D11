#include "../../Graphic.h"
#include "../../Shader/Shader.h"
#include "../../Shader/ShaderManager.h"
#include "../../Material/Material.h"
#include "DeferredRenderPipeline.h"
#include "GBufferPass.h"
#include "DeferredShadingResources.h"

namespace Framework 
{
	namespace DeferredShading 
	{
		GBufferPass::GBufferPass() : DeferredShadingPass()
		{

		}

		GBufferPass::~GBufferPass() 
		{
			DeferredShadingPass::~DeferredShadingPass();
		}

		void GBufferPass::Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps) 
		{
			DeferredShadingPass::Init(rp, rps);
		}

		void GBufferPass::OnFrameStart() 
		{

		}

		void GBufferPass::OnInvoke() 
		{
			UINT gbufferCnt = m_resouces->GetGBufferCnt();
			for (UINT g = 0; g < gbufferCnt; ++g) 
			{
				Framework::Graphics::ClearRenderTexture(m_resouces->GBuffer(g),Colors::Black);
			}
			//
			RenderingCameraInfo* cameraInfo = m_resouces->GetRenderingCameraInfo();
			RendererVector* renderers = m_resouces->GetOpaqueRenderes();
			//
			for (UINT g = 0; g < gbufferCnt; ++g)
			{
				DrawGBuffer(g, renderers,cameraInfo->viewMatrix, cameraInfo->projMatrix);
			}
			//
			Graphics::SetRenderTarget(nullptr);
			//
			size_t rendererCnt = renderers->size();
			for (size_t r = 0; r < rendererCnt; ++r)
			{
				Renderer* renderer = (*renderers)[r];
				Material* materialInst = renderer->GetMaterialInstance();
				Shader* shader = materialInst->GetShader();
				shader->SetEnabledTechnique("Default");
			}
		}

		void GBufferPass::DrawGBuffer(UINT index, RendererVector* renderers, XMMATRIX viewMatrix, XMMATRIX projMatrix)
		{
			Graphics::SetRenderTarget(m_resouces->GBuffer(index));
			Graphics::SetDepthStencil(m_resouces->GetCameraDepthTexture());
			Graphics::ClearDepthStencil();
			//
			size_t rendererCnt = renderers->size();
			for (size_t r = 0; r < rendererCnt; ++r) 
			{
				Renderer* renderer = (*renderers)[r];
				Material* materialInst = renderer->GetMaterialInstance();
				Shader* shader = materialInst->GetShader();
				shader->SetEnabledTechnique("GBuffer");
				m_renderPipeline->DrawRenderer(renderer, shader, viewMatrix, projMatrix,index);
			}
		}

	}
}