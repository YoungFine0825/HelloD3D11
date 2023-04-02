
#include "Global.h"
#include "App.h"

#ifdef AppImpl_HelloWorld

#include "Window.h"
#include "d3d/d3dGraphic.h"
#include "d3d/d3dEffectsUtil.h"
#include "math/MathLib.h"
#include "Color.h"

ID3DX11Effect* m_pEffect;

#define VERTEX_COUNT 8
#define INDEX_COUNT 36
#define VERTEX_FMT d3dGraphic::VertexFormat_Color
#define VERTEX_INPUT_DESC D3D_INPUT_DESC_COLOR

ID3D11Buffer* m_pVertexBuffer;
ID3D11Buffer* m_pIndexBuffer;
ID3D11InputLayout* m_pInputLayout;

XMMATRIX m_geoWorld;
XMMATRIX m_view;
XMMATRIX m_proj;

float m_geoYaw = 0;
XMFLOAT3 m_geoRotation = {0,0,0};

void App_PreCreateWindow() 
{

}

bool InitGemotry() 
{
	// Create vertex buffer
	VERTEX_FMT* vertices = new VERTEX_FMT[VERTEX_COUNT]
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), Colors::White   },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), Colors::Black   },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), Colors::Red     },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), Colors::Green   },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), Colors::Blue    },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), Colors::Yellow  },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), Colors::Cyan    },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), Colors::Magenta }
	};
	//
	if (!d3dGraphic::CreateVertexBuffer(&m_pVertexBuffer, vertices, sizeof(VERTEX_FMT), VERTEX_COUNT))
	{
		win_MsgBox(L"创建顶点缓存失败！");
		return false;
	}
	delete[] vertices;
	// Create the index buffer
	UINT* indices = new UINT[INDEX_COUNT] {
		// front face
		0, 1, 2,
		0, 2, 3,
		// back face
		4, 6, 5,
		4, 7, 6,
		// left face
		4, 5, 1,
		4, 1, 0,
		// right face
		3, 2, 6,
		3, 6, 7,
		// top face
		1, 5, 6,
		1, 6, 2,
		// bottom face
		4, 0, 3,
		4, 3, 7
	};
	if (!d3dGraphic::CreateIndexBuffer(&m_pIndexBuffer, indices, sizeof(UINT), INDEX_COUNT))
	{
		win_MsgBox(L"创建索引缓存失败！");
		return false;
	}
	delete[] indices;
	// Create the vertex input layout.
	if (!d3dGraphic::CreateInputLayout(&m_pInputLayout,VERTEX_INPUT_DESC,m_pEffect))
	{
		win_MsgBox(L"创建输入布局失败！");
		return false;
	}
	return true;
}

bool App_Init() 
{
	if (!d3dGraphic::Init()) 
	{
		return false;
	}
	//
	if (!d3dEffectsUtil::CreateEffectFromFxFile("res/effects/helloworld.fx", &m_pEffect)) 
	{
		win_MsgBox(L"加载Fx文件失败！");
		return false;
	}
	if (!InitGemotry()) 
	{
		return false;
	}
	//
	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(0, 0, -5, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_view = XMMatrixLookAtLH(pos, target, up);
	//
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	m_proj = XMMatrixPerspectiveFovLH(0.25f * PI, win_GetAspectRatio(), 1.0f, 1000.0f);
	//
	m_geoWorld = XMMatrixIdentity();
	//
	return true;
}

void App_Tick(float dt) 
{
	m_geoRotation.y += 20.0f * dt;
	
	m_geoWorld = XMMatrixRotationFromFloat3(m_geoRotation);
}

void App_Draw()
{
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	d3dGraphic::ClearRenderTarget(ClearColor);
	d3dGraphic::ClearDepthStencil();

	d3dGraphic::GetDeviceContext()->IASetInputLayout(m_pInputLayout);
	d3dGraphic::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX mvp = m_geoWorld * m_view * m_proj;
	d3dEffectsUtil::SetMatrix4x4(m_pEffect, "gMVP", mvp);

	UINT stride = sizeof(VERTEX_FMT);
	UINT offset = 0;
	d3dGraphic::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	d3dGraphic::GetDeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


	d3dEffectsUtil::DrawIndexed(m_pEffect, INDEX_COUNT);

	d3dGraphic::Present();
}

void App_Cleanup() 
{
	ReleaseCOM(m_pVertexBuffer);
	ReleaseCOM(m_pIndexBuffer);
	ReleaseCOM(m_pInputLayout);
	
	ReleaseCOM(m_pEffect);

	d3dGraphic::Shutdown();
}

#endif // AppImpl_HelloWorld
