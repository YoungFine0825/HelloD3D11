#pragma once
#include "d3d/d3dGraphic.h"

namespace Framework 
{
	typedef unsigned int RenderTextureId;

	struct RenderTextureInitInfo
	{
		RenderTextureId id = 0;
		unsigned int width;
		unsigned int height;
		ID3D11Texture2D* colorTex2D = nullptr;
		ID3D11RenderTargetView* colorBuffer = nullptr;
		ID3D11Texture2D* depthStencilTex2D = nullptr;
		ID3D11DepthStencilView* depthStencilBuffer = nullptr;
		ID3D11ShaderResourceView* colorSRV = nullptr;
		ID3D11UnorderedAccessView* colorUAV = nullptr;
		ID3D11ShaderResourceView* depthStencilSRV = nullptr;
		ID3D11UnorderedAccessView* depthStencilUAV = nullptr;
	};

	class RenderTexture
	{
	public:
		RenderTexture();
		RenderTexture(RenderTextureInitInfo info);
		~RenderTexture();
		//
		RenderTextureId GetId();
		ID3D11Texture2D* GetColorTexture2D();
		ID3D11Texture2D* GetDepthStencilTexture2D();
		ID3D11RenderTargetView* GetRTV();
		ID3D11DepthStencilView* GetDSV();
		ID3D11ShaderResourceView* GetColorTextureSRV();
		ID3D11ShaderResourceView* GetDepthTextureSRV();
		ID3D11UnorderedAccessView* GetColorTextureUAV();
		ID3D11UnorderedAccessView* GetDepthTextureUAV();
		unsigned int GetWidth();
		unsigned int GetHeight();
	private:
		RenderTextureId m_id;
		unsigned int m_width;
		unsigned int m_height;
		ID3D11Texture2D* m_colorTex2D = nullptr;
		ID3D11RenderTargetView * m_colorBuffer = nullptr;
		ID3D11ShaderResourceView* m_colorSRV = nullptr;
		ID3D11UnorderedAccessView* m_colorUAV = nullptr;
		//
		ID3D11Texture2D* m_depthStencilTex2D = nullptr;
		ID3D11DepthStencilView* m_depthStencilBuffer = nullptr;
		ID3D11ShaderResourceView* m_depthStencilSRV = nullptr;
		ID3D11UnorderedAccessView* m_depthStencilUAV = nullptr;
	};
}
