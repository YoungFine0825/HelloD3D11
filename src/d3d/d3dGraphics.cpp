
#include <cassert>
#include <vector>

#include "d3dGraphic.h"
#include "../Window.h"
#include "../Global.h"



namespace d3dGraphic 
{

	struct InputDesc
	{
		UINT eleCnt;
		D3D11_INPUT_ELEMENT_DESC vertexDesc[5];
	};

	#define INPUT_DESC_ARRAY_LENGTH 4
	InputDesc m_inputDescArray[INPUT_DESC_ARRAY_LENGTH] =
	{
		{
			1,
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
			}
		},
		{
			2,
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
			}
		},
		{
			2,
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
			}
		},
		{
			5,
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},//12字节
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},//12字节
				{"TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},//8字节
				{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},//16字节
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},//12字节
			}
		}
	};

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE
	};

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};

	std::vector <IDXGIAdapter*> m_adapters;
	unsigned int m_curAdaptersIndex = 0;

	ID3D11Device* m_pd3dDevice;
	ID3D11DeviceContext* m_pd3dImmediateContext;
	IDXGISwapChain* m_pSwapChain;

	D3D_FEATURE_LEVEL m_curFeatureLevel;
	UINT m_multiSampleCount = 4;
	UINT m_4xMsaaQuality;

	ID3D11Texture2D* m_pDefDepthStencilBuffer;
	ID3D11RenderTargetView* m_pDefRenderTargetView;
	ID3D11DepthStencilView* m_pDefDepthStencilView;
	D3D11_VIEWPORT m_defViewPort;

	bool m_enable4xMsaa = true;
	D3D_SHADING_MODE m_shadingMode = D3D_SHADING_MODE_SHADED;
	ID3D11RasterizerState* m_rsWireframeShading;
	ID3D11RasterizerState* m_rsNoCulling;

	ID3D11Device* GetDevice() { return m_pd3dDevice; }
	ID3D11DeviceContext* GetDeviceContext() { return m_pd3dImmediateContext; }
	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }

	ID3D11RenderTargetView* m_pSetRenderTargetView;
	ID3D11DepthStencilView* m_pSetDepthStencilView;

	void EnumerateAdapters() 
	{
		IDXGIAdapter* pAdapter;
		IDXGIFactory1* pFactory = NULL;
		// Create a DXGIFactory object.
		if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory)))
		{
			return;
		}
		for (UINT i = 0;
			pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND;
			++i)
		{
			m_adapters.push_back(pAdapter);
		}
		ReleaseCOM(pFactory);
	}


	bool CreateDevAndSwapChain() 
	{
		EnumerateAdapters();
		//
		if (m_adapters.size() <= 0) 
		{
			win_MsgBox(L"没有可用的显示适配器！");
			return false;
		}
		//
		UINT createDevFlag = 0;
#if defined(DEBUG) | defined(_DEBUG)
		//createDevFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		UINT numDriverTypes = ARRAYSIZE(driverTypes);
		UINT numFeatureLevels = ARRAYSIZE(featureLevels);
		//
		DXHR(D3D11CreateDevice(
			0,
			D3D_DRIVER_TYPE_HARDWARE,
			0,
			createDevFlag,
			featureLevels,
			numFeatureLevels,
			D3D11_SDK_VERSION,
			&m_pd3dDevice,
			&m_curFeatureLevel,
			&m_pd3dImmediateContext
		)
		);
		//
		DXHR(m_pd3dDevice->CheckMultisampleQualityLevels(
			DEFAULT_COLOR_BUFFER_FORMAT,
			m_multiSampleCount,
			&m_4xMsaaQuality
		));
		assert(m_4xMsaaQuality > 0);
		//
		IDXGIDevice* dxgiDevice;
		DXHR(m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));
		IDXGIAdapter* dxgiAdapter;
		DXHR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));
		IDXGIFactory* dxgiFactory;
		DXHR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));
		//
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = win_GetWidth();
		sd.BufferDesc.Height = win_GetHeight();
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = win_GetHandle();
		sd.Windowed = win_IsFullScreen() ? FALSE : TRUE;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		if (m_enable4xMsaa)
		{
			sd.SampleDesc.Count = 4;
			sd.SampleDesc.Quality = m_4xMsaaQuality - 1;
		}
		else
		{
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
		}
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;
		//
		DXHR(dxgiFactory->CreateSwapChain(
			m_pd3dDevice,
			&sd,
			&m_pSwapChain
		));
		//
		ReleaseCOM(dxgiDevice);
		ReleaseCOM(dxgiAdapter);
		ReleaseCOM(dxgiFactory);
		return true;
	}

	void ResizeScreen() 
	{
		assert(m_pd3dDevice);
		assert(m_pd3dImmediateContext);
		assert(m_pSwapChain);
		//
		ReleaseCOM(m_pDefRenderTargetView);
		ReleaseCOM(m_pDefDepthStencilView);
		ReleaseCOM(m_pDefDepthStencilBuffer);
		//
		DXHR(m_pSwapChain->ResizeBuffers(1, win_GetWidth(), win_GetHeight(), DEFAULT_COLOR_BUFFER_FORMAT, 0));
		//
		ID3D11Texture2D* pBackBuffer;
		DXHR(m_pSwapChain->GetBuffer(0,__uuidof(ID3D11Texture2D),(void**)&pBackBuffer));
		DXHR(m_pd3dDevice->CreateRenderTargetView(pBackBuffer, 0, &m_pDefRenderTargetView));
		ReleaseCOM(pBackBuffer);
		//
		D3D11_TEXTURE2D_DESC dsTexDesc;
		ZeroMemory(&dsTexDesc, sizeof(dsTexDesc));
		dsTexDesc.Width = win_GetWidth();
		dsTexDesc.Height = win_GetHeight();
		dsTexDesc.MipLevels = 1;
		dsTexDesc.ArraySize = 1;
		dsTexDesc.Format = DEFAULT_DS_BUFFER_FORMAT;
		if (m_enable4xMsaa)// Use 4X MSAA? --must match swap chain MSAA values.
		{
			dsTexDesc.SampleDesc.Count = 4;
			dsTexDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
		}
		else// No MSAA
		{
			dsTexDesc.SampleDesc.Count = 1;
			dsTexDesc.SampleDesc.Quality = 0;
		}
		dsTexDesc.Usage = D3D11_USAGE_DEFAULT;
		dsTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsTexDesc.CPUAccessFlags = 0;
		dsTexDesc.MiscFlags = 0;
		DXHR(m_pd3dDevice->CreateTexture2D(&dsTexDesc,0,&m_pDefDepthStencilBuffer));
		DXHR(m_pd3dDevice->CreateDepthStencilView(m_pDefDepthStencilBuffer, 0, &m_pDefDepthStencilView));
		//
		m_pd3dImmediateContext->OMSetRenderTargets(1, &m_pDefRenderTargetView, m_pDefDepthStencilView);
	}

	void ResetViewPort()
	{
		m_defViewPort.TopLeftX = 0;
		m_defViewPort.TopLeftY = 0;
		m_defViewPort.Width = static_cast<float>(win_GetWidth());
		m_defViewPort.Height = static_cast<float>(win_GetHeight());
		m_defViewPort.MinDepth = 0.0f;
		m_defViewPort.MaxDepth = 1.0f;
		m_pd3dImmediateContext->RSSetViewports(1, &m_defViewPort);
	}

	bool Init() 
	{
		if (!CreateDevAndSwapChain())
		{
			return false;
		}
		//
		ResizeScreen();
		ResetViewPort();
		//
		D3D11_RASTERIZER_DESC rsDesc;
		ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
		rsDesc.FillMode = D3D11_FILL_WIREFRAME;
		rsDesc.CullMode = D3D11_CULL_NONE;
		//rsDesc.FrontCounterClockwise = false;
		m_pd3dDevice->CreateRasterizerState(&rsDesc, &m_rsWireframeShading);
		//
		ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_NONE;
		//rsDesc.FrontCounterClockwise = false;
		m_pd3dDevice->CreateRasterizerState(&rsDesc, &m_rsNoCulling);
		//
		return true;
	}

	void Shutdown() 
	{
		ReleaseCOM(m_rsWireframeShading);
		ReleaseCOM(m_rsNoCulling);
		ReleaseCOM(m_pDefDepthStencilView);
		ReleaseCOM(m_pDefDepthStencilView);
		ReleaseCOM(m_pDefRenderTargetView);
		//
		ReleaseCOM(m_pSwapChain);
		ReleaseCOM(m_pd3dImmediateContext);
		ReleaseCOM(m_pd3dDevice);
		//
		for (size_t i = 0; i < m_adapters.size(); ++i) 
		{
			ReleaseCOM(m_adapters[i]);
		}
		m_adapters.clear();
	}

	void Present() 
	{
		m_pSwapChain->Present(0, 0);
	}

	void ClearRenderTarget(const FLOAT RGBA[4], ID3D11RenderTargetView* renderTarget)
	{
		if (renderTarget != nullptr) 
		{
			m_pd3dImmediateContext->ClearRenderTargetView(renderTarget, RGBA);
		}
		else 
		{
			m_pd3dImmediateContext->ClearRenderTargetView(m_pDefRenderTargetView, RGBA);
		}
	}

	void ClearDepthStencil(ID3D11DepthStencilView* dsView,UINT clearFlag) 
	{
		if (dsView != nullptr) 
		{
			m_pd3dImmediateContext->ClearDepthStencilView(dsView, clearFlag, 1.0f, 0);
		}
		else 
		{
			m_pd3dImmediateContext->ClearDepthStencilView(m_pDefDepthStencilView, clearFlag, 1.0f, 0);
		}
	}


	void SetRenderTarget(ID3D11RenderTargetView* renderTargetVIew, ID3D11DepthStencilView* depthStencilView) 
	{
		if (renderTargetVIew != nullptr) 
		{
			m_pSetRenderTargetView = renderTargetVIew;
			m_pSetDepthStencilView = depthStencilView;
			m_pd3dImmediateContext->OMSetRenderTargets(1, &m_pSetRenderTargetView, m_pSetDepthStencilView);
		}
		else 
		{
			m_pSetRenderTargetView = nullptr;
			m_pSetDepthStencilView = nullptr;
			m_pd3dImmediateContext->OMSetRenderTargets(1, &m_pDefRenderTargetView, m_pDefDepthStencilView);
		}
	}



	bool CreateVertexBuffer(ID3D11Buffer** buffer,const void* data, size_t vertexFormatSize, UINT vertexCount, D3D11_USAGE Usage)
	{
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = Usage;
		vbd.ByteWidth = vertexFormatSize * vertexCount;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		UINT accessFlag = 0;
		if (Usage == D3D11_USAGE_DYNAMIC) 
		{
			accessFlag |= D3D11_CPU_ACCESS_WRITE;
		}
		else if (Usage == D3D11_USAGE_STAGING)
		{
			accessFlag |= D3D11_CPU_ACCESS_WRITE;
			accessFlag |= D3D11_CPU_ACCESS_READ;
		}
		vbd.CPUAccessFlags = accessFlag;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		if (data != nullptr) 
		{
			D3D11_SUBRESOURCE_DATA vinitData;
			vinitData.pSysMem = data;
			if (FAILED(m_pd3dDevice->CreateBuffer(&vbd, &vinitData, buffer)))
			{
				return false;
			}
		}
		else 
		{
			if (FAILED(m_pd3dDevice->CreateBuffer(&vbd, nullptr, buffer)))
			{
				return false;
			}
		}
		return true;
	}

	bool CreateIndexBuffer(ID3D11Buffer** buffer, const void* data, size_t indexFormatSize, UINT indexCount, D3D11_USAGE Usage) 
	{
		D3D11_BUFFER_DESC ibd;
		ibd.Usage = Usage;
		ibd.ByteWidth = indexFormatSize * indexCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		UINT accessFlag = 0;
		if (Usage == D3D11_USAGE_DYNAMIC)
		{
			accessFlag |= D3D11_CPU_ACCESS_WRITE;
		}
		else if (Usage == D3D11_USAGE_STAGING) 
		{
			accessFlag |= D3D11_CPU_ACCESS_WRITE;
			accessFlag |= D3D11_CPU_ACCESS_READ;
		}
		ibd.CPUAccessFlags = accessFlag;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		if (data != nullptr)
		{
			D3D11_SUBRESOURCE_DATA iinitData;
			iinitData.pSysMem = data;
			if (FAILED(m_pd3dDevice->CreateBuffer(&ibd, &iinitData, buffer)))
			{
				return false;
			}
		}
		else 
		{
			if (FAILED(m_pd3dDevice->CreateBuffer(&ibd, nullptr, buffer)))
			{
				return false;
			}
		}
		return true;
	}

	bool CreateInputLayout(ID3DX11EffectPass* effPass, UINT inputIndex, ID3D11InputLayout** layout) 
	{
		if (effPass == nullptr || m_pd3dDevice == nullptr) 
		{
			return false;
		}
		if(inputIndex < 0 || inputIndex >= INPUT_DESC_ARRAY_LENGTH)
		{
			return false;
		}
		D3DX11_PASS_DESC passDesc;
		effPass->GetDesc(&passDesc);
		if (passDesc.pIAInputSignature == nullptr && passDesc.IAInputSignatureSize <= 0) 
		{
			//不需要创建顶点输入布局
			return true;
		}
		//
		InputDesc inputDesc = m_inputDescArray[inputIndex];
		if (FAILED(m_pd3dDevice->CreateInputLayout(
			inputDesc.vertexDesc, 
			inputDesc.eleCnt, 
			passDesc.pIAInputSignature,
			passDesc.IAInputSignatureSize, 
			layout)
			)
		)
		{
			return false;
		}
		return true;
	}

	bool CreateInputLayout(ID3D11InputLayout** layout, UINT inputIndex, ID3DX11Effect* effect)
	{
		ID3DX11EffectTechnique* tech = effect->GetTechniqueByIndex(0);
		if (tech == nullptr) { return false; }
		//
		ID3DX11EffectPass* pass = tech->GetPassByIndex(0);
		//
		return CreateInputLayout(pass, inputIndex, layout);
	}


	bool SetShadingMode(D3D_SHADING_MODE mode) 
	{
		if (mode == D3D_SHADING_MODE_WIREFRAME )
		{
			if (m_rsWireframeShading == nullptr) 
			{
				return false;
			}
			m_pd3dImmediateContext->RSSetState(m_rsWireframeShading);
		}
		else 
		{
			m_pd3dImmediateContext->RSSetState(0);
		}
		m_shadingMode = mode;
		return true;
	}

	void EnableBackCulling(bool enable) 
	{
		if (enable) 
		{
			m_pd3dImmediateContext->RSSetState(0);
		}
		else 
		{
			m_pd3dImmediateContext->RSSetState(m_rsNoCulling);
		}
	}
}