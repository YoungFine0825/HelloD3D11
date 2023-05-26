
#include "Graphic.h"
#include "../Window.h"
#include "../math/MathLib.h"
#include "Texture/Texture.h"
#include "RenderTexture/RenderTexture.h"
#include "Mesh/Mesh.h"
#include "Mesh/MeshManager.h"
#include "Shader/Shader.h"
#include "Shader/ShaderManager.h"

namespace Framework 
{
	namespace Graphics 
	{
		ID3D11RenderTargetView* m_activedRenderTargetView = nullptr;
		ID3D11DepthStencilView* m_activedDepthStencilView = nullptr;
		RenderTexture* m_activedRenderTexture = nullptr;

		Shader* m_blitShader = nullptr;
		RenderTexture* m_lastRTBeforeBlit = nullptr;

		void ClearBackground(RGBA32 bgColor) 
		{
			d3dGraphic::ClearRenderTarget((const float*)&bgColor, m_activedRenderTargetView);
		}

		void ClearDepthStencil(UINT clearFlag)
		{
			d3dGraphic::ClearDepthStencil(m_activedDepthStencilView, clearFlag);
		}

		void ClearRenderTexture(Framework::RenderTexture* rt, const RGBA32 bgColor, UINT clearFlag) 
		{
			if (rt->GetRTV()) 
			{
				d3dGraphic::ClearRenderTarget((const float*)&bgColor, rt->GetRTV());
			}
			if (rt->GetDSV()) 
			{
				d3dGraphic::ClearDepthStencil(rt->GetDSV(), clearFlag);
			}
		}

		void Present() 
		{
			d3dGraphic::Present();
		}

		void SetRenderTarget(RenderTexture* rt) 
		{
			m_activedRenderTexture = rt;
			if (rt == nullptr) 
			{
				m_activedRenderTargetView = nullptr;
				m_activedDepthStencilView = nullptr;
				d3dGraphic::SetViewPort(nullptr);
				d3dGraphic::SetRenderTarget(nullptr, nullptr);
				return;
			}
			ID3D11RenderTargetView* rtv = rt->GetRTV();
			ID3D11DepthStencilView* dsv = rt->GetDSV();
			m_activedRenderTargetView = rtv;
			m_activedDepthStencilView = dsv;
			d3dGraphic::SetViewPort(rt->GetViewPortPtr());
			d3dGraphic::SetRenderTarget(rtv, dsv);
		}

		void SetDepthStencil(Framework::RenderTexture* depthStencil)
		{
			if (depthStencil == nullptr || depthStencil->GetDSV() == nullptr)
			{
				if (m_activedRenderTexture) 
				{
					m_activedDepthStencilView = m_activedRenderTexture->GetDSV();
				}
				else 
				{
					m_activedDepthStencilView = nullptr;
				}
			}
			else 
			{
				m_activedDepthStencilView = depthStencil->GetDSV();
			}
			d3dGraphic::SetRenderTarget(m_activedRenderTargetView, m_activedDepthStencilView);
		}

		void DrawMesh(Mesh* mesh, Shader* shader,int pass) 
		{
			if (mesh == nullptr || shader == nullptr)
			{
				return;
			}
			ID3D11DeviceContext* context = d3dGraphic::GetDeviceContext();
			if (context == nullptr) 
			{ 
				return;
			}
			ID3D11Buffer* vb = mesh->GetVertexBuffer();
			ID3D11Buffer* ib = mesh->GetIndexBuffer();
			if (vb == nullptr || ib == nullptr)
			{
				return;
			}
			UINT stride = sizeof(d3dGraphic::VertexFormat_Common);
			UINT offset = 0;
			context->IASetPrimitiveTopology(mesh->GetPrimitiveTopology());
			context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
			context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
			//
			UINT passCount = shader->GetPassCount();
			if (pass < 0 || pass >= passCount) 
			{
				for (UINT p = 0; p < passCount; ++p)
				{
					context->IASetInputLayout(shader->GetInputLayout(p));
					shader->ApplyPass(p, context);
					context->DrawIndexed(mesh->GetIndicesNumber(), 0, 0);
				}
			}
			else 
			{
				context->IASetInputLayout(shader->GetInputLayout(pass));
				shader->ApplyPass(pass, context);
				context->DrawIndexed(mesh->GetIndicesNumber(), 0, 0);
			}
		}

		void Blit(ID3D11ShaderResourceView* srv, Framework::RenderTexture* dst) 
		{
			if (srv == nullptr)
			{
				return;
			}
			if (m_blitShader == nullptr)
			{
				m_blitShader = ShaderManager::LoadFromFxFile("res/effects/BlitCopy.fx");
				if (m_blitShader == nullptr)
				{
					return;
				}
			}
			//
			unsigned int dstWid = 0;
			unsigned int dstHei = 0;
			if (dst != nullptr)
			{
				dstWid = dst->GetWidth();
				dstHei = dst->GetHeight();
			}
			else
			{
				dstWid = win_GetWidth();
				dstHei = win_GetHeight();
			}
			//
			float aspectRadtio = (float)dstWid / (float)dstHei;
			Mesh* triangle = MeshManager::CreateFullScreenTriangle();
			XMMATRIX translate = XMMatrixTranslationFromFloat3({ aspectRadtio / 2 * -1,  0.5,  2 });
			XMMATRIX scaling = XMMatrixScaling(aspectRadtio, 1, 1);
			XMMATRIX triangleWorldMat = scaling * translate;
			XMMATRIX proj = XMMatrixOrthographicLH(aspectRadtio, 1, 1, 3);
			m_blitShader->SetMatrix4x4("obj_MatMVP", triangleWorldMat * proj);
			m_blitShader->SetShaderResourceView("g_diffuseMap", srv);
			bool hasDepth = dst == nullptr || dst->GetDSV() != nullptr;
			//
			SetRenderTarget(dst);
			ClearBackground(Colors::Black);
			if (hasDepth)
			{
				ClearDepthStencil();
			}
			DrawMesh(triangle, m_blitShader);
			//
			SetRenderTarget(nullptr);
		}

		void Blit(RenderTexture* src, RenderTexture* dst) 
		{
			if (src->GetColorTextureSRV() == nullptr) 
			{
				return;
			}
			Blit(src->GetColorTextureSRV(), dst);
		}

		void Blit(Framework::Texture* texture, Framework::RenderTexture* dst) 
		{
			if (texture->GetShaderResourceView() == nullptr) 
			{
				return;
			}
			Blit(texture->GetShaderResourceView(), dst);
		}
	}
}