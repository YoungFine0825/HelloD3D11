#include <unordered_map>

#include "Texture.h"
#include "TextureManager.h"
#include "../Graphic.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


namespace Framework 
{
	namespace TextureManager 
	{
		typedef std::unordered_map<Resource::ResourceUrl, Texture*> TextureResMap;
		TextureResMap m_texResMap;

		Texture* FindWithUrl(Resource::ResourceUrl url)
		{
			TextureResMap::iterator it = m_texResMap.find(url);
			if (it == m_texResMap.end())
			{
				return nullptr;
			}
			return it->second;
		}

		void Cleanup()
		{
			TextureResMap::iterator it;
			for (it = m_texResMap.begin(); it != m_texResMap.end(); ++it)
			{
				Resource::ReleaseWithGUID(it->second->GetGUID());
			}
			m_texResMap.clear();
		}

		Texture* LoadDDSFromFile(Resource::ResourceUrl url) 
		{

			Texture* tex = FindWithUrl(url);
			if (tex != nullptr) 
			{
				return tex;
			}

			ID3D11Texture2D* pTexture2D = NULL;

			// Load the texture and initialize an ID3D11Texture2D object.
			HRESULT hr = D3DX11CreateTextureFromFileA(d3dGraphic::GetDevice(), url.c_str(), NULL, NULL, (ID3D11Resource**)&pTexture2D, NULL);
			if (FAILED(hr)) 
			{
				return nullptr;
			}

			tex = new Texture();
			tex->SetResource(pTexture2D);
			m_texResMap[url] = tex;

			return tex;
		}

		size_t getMemSize(unsigned int width, unsigned int height)
		{
			return ((width + 3) / 4) * ((height + 3) / 4) * 8;
		}

		Texture* LoadFromFile(Resource::ResourceUrl url) 
		{
			Texture* tex = FindWithUrl(url);
			if (tex != nullptr)
			{
				return tex;
			}
			int width = 0;
			int height = 0;
			int colorChannels = 0;
			int requiredChannels = 4;
			unsigned char* data = stbi_load(url.c_str(), &width, &height, &colorChannels, requiredChannels);
			if (data == nullptr || data == 0)
			{
				return nullptr;
			}
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));//不是每个字段都赋值的话，这一步很重要
			desc.Width = static_cast<UINT>(width);
			desc.Height = static_cast<UINT>(height);
			desc.CPUAccessFlags = 0;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = data;
			initData.SysMemPitch = width * requiredChannels;
			initData.SysMemSlicePitch = 0;
			ID3D11Texture2D* pTexture2D = NULL;
			DXHR(d3dGraphic::GetDevice()->CreateTexture2D(&desc, &initData, &pTexture2D));
			if (!pTexture2D) 
			{
				stbi_image_free(data);
				return nullptr;
			}
			tex = new Texture();
			tex->SetResource(pTexture2D);
			m_texResMap[url] = tex;
			stbi_image_free(data);
			return tex;
		}
	}
}