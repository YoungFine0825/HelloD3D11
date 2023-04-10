#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <DxErr.h>
#include <d3dx11effect.h>
#include <D3DX11tex.h>

#include "../math/MathLib.h"

#if defined(DEBUG) | defined(_DEBUG)
#ifndef DXHR
#define DXHR(x)											\
	{														\
		HRESULT hr=(x);										\
		if(FAILED(hr))										\
		{													\
			DXTrace(__FILE__,(DWORD)__LINE__,hr,L#x,true);	\
		}													\
	}
#endif // !DXHR
#else
#ifndef DXHR
#define DXHR(x) (x)
#endif
#endif

namespace d3dGraphic
{
	#define DEFAULT_COLOR_BUFFER_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
	#define DEFAULT_DS_BUFFER_FORMAT DXGI_FORMAT_D24_UNORM_S8_UINT

	#define D3D_INPUT_DESC_BASE 0
	#define D3D_INPUT_DESC_COLOR 1
	#define D3D_INPUT_DESC_TEXTURED 2
	#define D3D_INPUT_DESC_COMMON 3

	enum D3D_SHADING_MODE {
		D3D_SHADING_MODE_SHADED,
		D3D_SHADING_MODE_WIREFRAME,
	};

	struct VertexFormat_Base
	{
		VertexFormat_Base() {};
		VertexFormat_Base(XMFLOAT3 pos) : position(pos) {};
		XMFLOAT3 position;
	};
	struct VertexFormat_Color :VertexFormat_Base
	{
		VertexFormat_Color() : VertexFormat_Base() {};
		VertexFormat_Color(XMFLOAT3 pos, XMFLOAT4 col) :VertexFormat_Base(pos), color(col) {};
		XMFLOAT4 color;
	};
	struct VertexFormat_Textured :VertexFormat_Base
	{
		VertexFormat_Textured() : VertexFormat_Base() {};
		VertexFormat_Textured(XMFLOAT3 pos, XMFLOAT2 uv) :VertexFormat_Base(pos), textureCoord0(uv) {};
		XMFLOAT2 textureCoord0;
	};
	struct VertexFormat_Common :VertexFormat_Base
	{
		VertexFormat_Common() : VertexFormat_Base() {};
		VertexFormat_Common(XMFLOAT3 pos, XMFLOAT3 nor, XMFLOAT2 uv, XMFLOAT4 col, XMFLOAT3 tan) :
			VertexFormat_Base(pos), normal(nor), textureCoord0(uv),color(col),tangent(tan)
		{};
		XMFLOAT3 normal;
		XMFLOAT2 textureCoord0;
		XMFLOAT4 color;
		XMFLOAT3 tangent;
	};

	typedef D3D11_VIEWPORT ViewPort;

	bool Init();
	void Shutdown();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	IDXGISwapChain* GetSwapChain();
	//
	void Present();
	void ClearRenderTarget(const FLOAT RGBA[4],ID3D11RenderTargetView* renderTarget = nullptr);
	void ClearDepthStencil(ID3D11DepthStencilView* stencil = nullptr,UINT clearFlag = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
	void SetRenderTarget(ID3D11RenderTargetView* renderTargetVIew, ID3D11DepthStencilView* depthStencilView);
	void SetViewPort(ViewPort* viewPortPtr);
	//
	bool CreateVertexBuffer(ID3D11Buffer** buffer,const void* data, size_t vertexFormatSize, UINT vertexCount, D3D11_USAGE Usage = D3D11_USAGE_IMMUTABLE);
	bool CreateIndexBuffer(ID3D11Buffer** buffer, const void* data, size_t indexFormatSize, UINT indexCount, D3D11_USAGE Usage = D3D11_USAGE_IMMUTABLE);
	bool CreateInputLayout(ID3DX11EffectPass* effPass, UINT inputIndex, ID3D11InputLayout** layout);
	bool CreateInputLayout(ID3D11InputLayout** layout, UINT inputIndex,ID3DX11Effect* effect);
	//
	bool SetShadingMode(D3D_SHADING_MODE mode);
	void EnableBackCulling(bool enable);
}