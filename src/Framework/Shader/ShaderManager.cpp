
#include <unordered_map>
#include "../Graphic.h"

#include "Shader.h"
#include "ShaderManager.h"

namespace Framework 
{
	namespace ShaderManager 
	{
		typedef std::unordered_map<Resource::ResourceUrl, Shader*> ShaderResMap;
		ShaderResMap m_shaderResMap;

		Shader* FindWithUrl(Resource::ResourceUrl url) 
		{
			ShaderResMap::iterator it = m_shaderResMap.find(url);
			if (it == m_shaderResMap.end()) 
			{
				return nullptr;
			}
			return it->second;
		}

		Shader* LoadFromFxFile(Resource::ResourceUrl url) 
		{
			ID3D10Blob* compiledShader;
			ID3D10Blob* errMsg;
			DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
			shaderFlags |= D3D10_SHADER_DEBUG;
			shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
			HRESULT hr = D3DX11CompileFromFileA(url.c_str(), 0, 0, 0, "fx_5_0", shaderFlags,
				0, 0, &compiledShader, &errMsg, 0);

			// compilationMsgs can store errors or warnings.
			if (errMsg != nullptr)
			{
				MessageBoxA(0, (char*)errMsg->GetBufferPointer(), 0, 0);
				errMsg->Release();
				errMsg = nullptr;
			}
			// Even if there are no compilationMsgs, check to make sure there were no other errors.
			if (FAILED(hr))
			{
				DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
				return nullptr;
			}

			Shader* shader = FindWithUrl(url);
			if (shader == nullptr)
			{
				shader = new Shader();
				shader->SetUrl(url);
				if (!shader->CreateFromBlob(compiledShader)) 
				{
					delete shader;
					return nullptr;
				}
				m_shaderResMap[url] = shader;
			}
			else 
			{
				if (!shader->CreateFromBlob(compiledShader))
				{
					return nullptr;
				}
			}
			return shader;
		}

		void Cleanup() 
		{
			ShaderResMap::iterator it;
			for (it = m_shaderResMap.begin(); it != m_shaderResMap.end(); ++it)
			{
				Resource::ReleaseWithGUID(it->second->GetGUID());
			}
			m_shaderResMap.clear();
		}
	}
}