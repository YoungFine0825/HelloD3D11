#include <unordered_map>

#include "Texture.h"
#include "TextureManager.h"
#include "../Graphic.h"


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
	}
}