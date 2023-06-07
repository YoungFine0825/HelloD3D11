#include "../../Global.h"
#include "RenderTexture.h"

namespace Framework 
{
	RenderTexture::RenderTexture() 
	{

	}

	RenderTexture::RenderTexture(RenderTextureInitInfo info) 
	{
		m_id = info.id;
		m_width = info.width;
		m_height = info.height;
		m_colorTex2D = info.colorTex2D;
		m_colorBuffer = info.colorBuffer;
		m_colorSRV = info.colorSRV;
		m_colorUAV = info.colorUAV;
		m_depthStencilTex2D = info.depthStencilTex2D;
		m_depthStencilBuffer = info.depthStencilBuffer;
		m_depthStencilSRV = info.depthStencilSRV;
		m_depthStencilUAV = info.depthStencilUAV;
		//
		m_viewPortPtr = new d3dGraphic::ViewPort();
		m_viewPortPtr->TopLeftX = 0;
		m_viewPortPtr->TopLeftY = 0;
		m_viewPortPtr->Width = static_cast<float>(m_width);
		m_viewPortPtr->Height = static_cast<float>(m_height);
		m_viewPortPtr->MinDepth = 0.0f;
		m_viewPortPtr->MaxDepth = 1.0f;
	}

	RenderTexture::~RenderTexture() 
	{
		ReleasePointer(m_viewPortPtr);
		ReleaseCOM(m_colorUAV);
		ReleaseCOM(m_depthStencilUAV);
		ReleaseCOM(m_colorSRV);
		ReleaseCOM(m_depthStencilSRV);
		ReleaseCOM(m_colorBuffer);
		ReleaseCOM(m_depthStencilBuffer);
		ReleaseCOM(m_colorTex2D);
		ReleaseCOM(m_depthStencilTex2D);
	}

	RenderTextureId RenderTexture::GetId() { return m_id; }
	ID3D11Texture2D* RenderTexture::GetColorTexture2D() { return m_colorTex2D; }
	ID3D11Texture2D* RenderTexture::GetDepthStencilTexture2D() { return m_depthStencilTex2D; }
	ID3D11RenderTargetView* RenderTexture::GetRTV() { return m_colorBuffer; }
	ID3D11DepthStencilView* RenderTexture::GetDSV() { return m_depthStencilBuffer; }
	ID3D11ShaderResourceView* RenderTexture::GetColorTextureSRV() { return m_colorSRV; }
	ID3D11ShaderResourceView* RenderTexture::GetDepthTextureSRV() { return m_depthStencilSRV; }
	ID3D11UnorderedAccessView* RenderTexture::GetColorTextureUAV() { return m_colorUAV; }
	ID3D11UnorderedAccessView* RenderTexture::GetDepthTextureUAV() { return m_depthStencilUAV; }
	unsigned int RenderTexture::GetWidth() { return m_width; }
	unsigned int RenderTexture::GetHeight() { return m_height; }
	d3dGraphic::ViewPort* RenderTexture::GetViewPortPtr() { return m_viewPortPtr; }

	RenderTexture* RenderTexture::SetViewport(d3dGraphic::ViewPort* viewPort) 
	{
		if (!viewPort || !m_viewPortPtr) 
		{
			return this;
		}
		m_viewPortPtr->TopLeftX = viewPort->TopLeftX;
		m_viewPortPtr->TopLeftY = viewPort->TopLeftY;
		m_viewPortPtr->Width = viewPort->Width;
		m_viewPortPtr->Height = viewPort->Height;
		m_viewPortPtr->MinDepth = viewPort->MinDepth;
		m_viewPortPtr->MaxDepth = viewPort->MaxDepth;
		return this;
	}
}