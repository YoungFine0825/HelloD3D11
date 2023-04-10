#include <unordered_map>

#include "RenderTextureManager.h"

namespace Framework 
{
	namespace RenderTextureManager 
	{
		RenderTextureId g_rtId = 0;

		typedef std::unordered_map<RenderTextureId, RenderTexture*> RTMap;
		RTMap m_rtMap;

		RenderTexture* FindRenderTexture(RenderTextureId id) 
		{
			RTMap::iterator it = m_rtMap.find(id);
			if (it == m_rtMap.end())
			{
				return nullptr;
			}
			return it->second;
		}

		void Cleanup() 
		{
			RTMap::iterator it;
			for (it = m_rtMap.begin(); it != m_rtMap.end(); ++it)
			{
				delete it->second;
			}
			m_rtMap.clear();
		}

		RenderTexture* CreateRenderTexture(
			unsigned int width,
			unsigned int height,
			bool includeDepthStencil,
			bool createColorUAV,
			bool createDepthUAV,
			unsigned int msaaCount
		) 
		{
			RenderTextureDesc desc;
			desc.width = width;
			desc.height = height;
			desc.includeDepthStencil = includeDepthStencil;
			desc.createColorUAV = createColorUAV;
			desc.createDepthUAV = createDepthUAV;
			desc.msaaCount = msaaCount;
			return CreateRenderTexture(&desc);

		}

		RenderTexture* CreateRenderTexture(RenderTextureDesc* rtDesc)
		{
			ID3D11Device* device = d3dGraphic::GetDevice();
			ID3D11DeviceContext* context = d3dGraphic::GetDeviceContext();
			//
			D3D11_TEXTURE2D_DESC texDesc;
			ZeroMemory(&texDesc, sizeof(texDesc));
			texDesc.Width = rtDesc->width;
			texDesc.Height = rtDesc->height;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			if (rtDesc->msaaCount > 1)// Use 4X MSAA? --must match swap chain MSAA values.
			{
				rtDesc->msaaCount = min(rtDesc->msaaCount, 8);
				texDesc.SampleDesc.Count = rtDesc->msaaCount;
				texDesc.SampleDesc.Quality = rtDesc->msaaCount - 1;
			}
			else// No MSAA
			{
				texDesc.SampleDesc.Count = 1;
				texDesc.SampleDesc.Quality = 0;
			}
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.CPUAccessFlags = 0;
			texDesc.MiscFlags = 0;
			//
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;
			//
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = 0;
			//
			ID3D11Texture2D* colorTex2D = nullptr;
			ID3D11RenderTargetView* colorBuffer = nullptr;
			ID3D11Texture2D* depthStencilTex2D = nullptr;
			ID3D11DepthStencilView* depthStencilBuffer = nullptr;
			ID3D11ShaderResourceView* colorSRV = nullptr;
			ID3D11ShaderResourceView* depthStencilSRV = nullptr;
			ID3D11UnorderedAccessView* colorUAV = nullptr;
			ID3D11UnorderedAccessView* depthStencilUAV = nullptr;
			//
			if (rtDesc->includeColor) 
			{
				texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				if (rtDesc->createColorUAV)
				{
					texDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
				}
				DXHR(device->CreateTexture2D(&texDesc, 0, &colorTex2D));
				DXHR(device->CreateRenderTargetView(colorTex2D, 0, &colorBuffer));
				//
				srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				DXHR(device->CreateShaderResourceView(colorTex2D, &srvDesc, &colorSRV));
				//
				if (rtDesc->createColorUAV)
				{
					uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					DXHR(device->CreateUnorderedAccessView(colorTex2D, &uavDesc, &colorUAV));
				}
			}
			//
			if (rtDesc->includeDepthStencil)
			{
				texDesc.Format = DEFAULT_DS_BUFFER_FORMAT;
				texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL ;
				if (rtDesc->createDepthUAV)
				{
					texDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
				}
				DXHR(device->CreateTexture2D(&texDesc, 0, &depthStencilTex2D));
				DXHR(device->CreateDepthStencilView(depthStencilTex2D, 0, &depthStencilBuffer));
				//
				if (texDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE) 
				{
					srvDesc.Format = DEFAULT_DS_BUFFER_FORMAT;
					DXHR(device->CreateShaderResourceView(depthStencilTex2D, &srvDesc, &depthStencilSRV));
				}
				//
				if (rtDesc->createDepthUAV)
				{
					uavDesc.Format = DEFAULT_DS_BUFFER_FORMAT;
					DXHR(device->CreateUnorderedAccessView(depthStencilTex2D, &uavDesc, &depthStencilUAV));
				}
			}
			//
			RenderTextureId id = ++g_rtId;
			RenderTextureInitInfo initInfo;
			initInfo.id = id;
			initInfo.width = rtDesc->width;
			initInfo.height = rtDesc->height;
			initInfo.colorTex2D = colorTex2D;
			initInfo.colorBuffer = colorBuffer;
			initInfo.colorSRV = colorSRV;
			initInfo.colorUAV = colorUAV;
			initInfo.depthStencilTex2D = depthStencilTex2D;
			initInfo.depthStencilBuffer = depthStencilBuffer;
			initInfo.depthStencilSRV = depthStencilSRV;
			initInfo.depthStencilUAV = depthStencilUAV;
			//
			RenderTexture* rt = new RenderTexture(initInfo);
			m_rtMap[id] = rt;
			return rt;
		}


		void ReleaseRenderTexture(RenderTextureId id) 
		{
			RTMap::iterator it = m_rtMap.find(id);
			if (it == m_rtMap.end())
			{
				return;
			}
			delete it->second;
			m_rtMap.erase(it);
		}


		void ReleaseRenderTexture(RenderTexture* rt) 
		{
			ReleaseRenderTexture(rt->GetId());
		}
	}
}