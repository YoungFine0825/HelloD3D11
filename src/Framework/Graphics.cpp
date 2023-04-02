
#include "Graphic.h"
#include "../Window.h"
#include "../math/MathLib.h"

#include "Mesh/MeshManager.h"
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
				d3dGraphic::SetRenderTarget(nullptr, nullptr);
				return;
			}
			ID3D11RenderTargetView* rtv = rt->GetRTV();
			ID3D11DepthStencilView* dsv = rt->GetDSV();
			m_activedRenderTargetView = rtv;
			m_activedDepthStencilView = dsv;
			d3dGraphic::SetRenderTarget(rtv, dsv);
		}

		void DrawMesh(Mesh* mesh, Shader* shader) 
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
			context->IASetInputLayout(shader->GetInputLayout());
			//
			UINT passCount = shader->GetPassCount();
			for (UINT p = 0; p < passCount; ++p)
			{
				shader->ApplyPass(p, context);
				context->DrawIndexed(mesh->GetIndicesNumber(), 0, 0);
			}
		}

		void Blit(RenderTexture* src, RenderTexture* dst) 
		{
			if (src->GetColorTextureSRV() == nullptr) 
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
			m_blitShader->SetShaderResourceView("g_diffuseMap", src->GetColorTextureSRV());
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
	}
}