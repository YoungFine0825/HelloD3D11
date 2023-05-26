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
		GBufferPass::GBufferPass() 
		{

		}

		GBufferPass::~GBufferPass() 
		{

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
			Camera* renderingCamera = m_renderPipeline->GetCurRenderingCamera();
			XMMATRIX viewMat = renderingCamera->GetViewMatrix();
			XMMATRIX projMat = renderingCamera->GetProjectMatrix();
			//
			for (UINT g = 0; g < gbufferCnt; ++g)
			{
				DrawGBuffer(g, nullptr, viewMat, projMat);
			}
			//
			Graphics::SetRenderTarget(nullptr);
			Graphics::Blit(m_resouces->GBuffer(1), nullptr);
		}

		void GBufferPass::DrawGBuffer(UINT index, Shader* shader, XMMATRIX viewMatrix, XMMATRIX projMatrix)
		{
			Graphics::SetRenderTarget(m_resouces->GBuffer(index));
			Graphics::SetDepthStencil(m_resouces->GetCameraDepthTexture());
			Graphics::ClearDepthStencil();
			//
			RendererVector* renderers = m_resouces->GetVisibleOpaqueRenderes();
			size_t rendererCnt = renderers->size();
			for (size_t r = 0; r < rendererCnt; ++r) 
			{
				Renderer* renderer = (*renderers)[r];
				Material* materialInst = renderer->GetMaterialInstance();
				if (!materialInst->IsEnableLighting()) 
				{
					continue;
				}
				Shader* shader = materialInst->GetShader();
				if (!shader->hasTechnique("GBuffer")) 
				{
					continue;
				}
				shader->SetEnabledTechnique("GBuffer");
				m_renderPipeline->DrawRenderer(renderer, shader, viewMatrix, projMatrix,index);
			}
		}

	}
}