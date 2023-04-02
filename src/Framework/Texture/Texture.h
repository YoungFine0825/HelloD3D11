#pragma once

#include "../Resource.h"

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

namespace Framework 
{
	class Texture : public Resource::IResource
	{
	public:
		Texture();
		~Texture() override;
		//
		void Release() override;
		//
		bool SetResource(ID3D11Texture2D* texSrc);
		ID3D11Texture2D* GetResource();
		ID3D11ShaderResourceView* GetShaderResourceView();
		unsigned int GetWidth();
		unsigned int GetHeight();
		unsigned int GetMipLevels();

	private:
		ID3D11Texture2D* m_pTexture2D = NULL;
		ID3D11ShaderResourceView* m_pSRView = NULL;
		//
		unsigned int m_width = 0;
		unsigned int m_height = 0;
		unsigned int m_mipLevels = 0;
	};
}