#include "ShadowMap.h"
#include "../../Global.h"

namespace Framework 
{
	ShadowMap::ShadowMap() 
	{

	}

	ShadowMap::~ShadowMap() 
	{
		ReleaseCOM(m_depthMapDSV);
		ReleaseCOM(m_depthMapSRV);
	}

	ShadowMap* ShadowMap::SetSize(unsigned int size)
	{
		if (m_size == size)
		{
			return this;
		}
		ReleaseCOM(m_depthMapDSV);
		ReleaseCOM(m_depthMapSRV);
		//
		m_size = size;
		//
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width = static_cast<float>(size);
		m_viewport.Height = static_cast<float>(size);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		// Use typeless format because the DSV is going to interpret
		// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going to interpret
		// the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = size;
		texDesc.Height = size;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		ID3D11Device* device = d3dGraphic::GetDevice();
		ID3D11Texture2D* depthMap = 0;
		DXHR(device->CreateTexture2D(&texDesc, 0, &depthMap));

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = 0;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		DXHR(device->CreateDepthStencilView(depthMap, &dsvDesc, &m_depthMapDSV));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		DXHR(device->CreateShaderResourceView(depthMap, &srvDesc, &m_depthMapSRV));

		// View saves a reference to the texture so we can release our reference.
		ReleaseCOM(depthMap);

		return this;
	}

	ID3D11DepthStencilView* ShadowMap::GetDSV() 
	{
		return m_depthMapDSV;
	}

	ID3D11ShaderResourceView* ShadowMap::GetSRV() 
	{
		return m_depthMapSRV;
	}

	D3D11_VIEWPORT* ShadowMap::GetViewport() 
	{
		return &m_viewport;
	}

	XMMATRIX ShadowMap::GetViewProjectMatrix() 
	{
		return m_viewProjectMatrix;
	}

	unsigned int ShadowMap::GetSize() 
	{
		return m_size;
	}
}