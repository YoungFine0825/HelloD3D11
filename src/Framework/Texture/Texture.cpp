
#include "../../Global.h"
#include "../Graphic.h"
#include "Texture.h"


namespace Framework
{
	Texture::Texture() :IResource()
	{

	}

	Texture::~Texture()
	{
		IResource::~IResource();
	}

	void Texture::Release()
	{
		ReleaseCOM(m_pSRView);
		ReleaseCOM(m_pTexture2D);
		IResource::Release();
	}

	bool Texture::SetResource(ID3D11Texture2D* texSrc)
	{
		m_pTexture2D = texSrc;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		D3D11_TEXTURE2D_DESC m_desc;

		// Get a texture description to determine the texture
// format of the loaded texture.
		texSrc->GetDesc(&m_desc);

		// Fill in the D3D11_SHADER_RESOURCE_VIEW_DESC structure.
		srvDesc.Format = m_desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = m_desc.MipLevels;

		if (m_pSRView != nullptr)
		{
			ReleaseCOM(m_pSRView);
		}

		m_width = m_desc.Width;
		m_height = m_desc.Height;
		m_mipLevels = m_desc.MipLevels;

		// Create the shader resource view.
		d3dGraphic::GetDevice()->CreateShaderResourceView(texSrc, &srvDesc, &m_pSRView);
		return true;
	}

	ID3D11Texture2D* Texture::GetResource()
	{
		return m_pTexture2D;
	}

	ID3D11ShaderResourceView* Texture::GetShaderResourceView() 
	{
		return m_pSRView;
	}

	unsigned int Texture::GetWidth()
	{
		return m_width;
	}

	unsigned int Texture::GetHeight()
	{
		return m_height;
	}

	unsigned int Texture::GetMipLevels()
	{
		return m_mipLevels;
	}
}