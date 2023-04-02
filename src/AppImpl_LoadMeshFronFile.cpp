
#include "Global.h"
#include "App.h"

#ifdef AppImpl_LoadMeshFromFile

#include "Window.h"
#include "math/MathLib.h"
#include "Color.h"

#include "Framework/Graphic.h"
#include "Framework/Mesh/MeshManager.h"
#include "Framework/Shader/ShaderManager.h"

using namespace Framework;

XMMATRIX m_geoWorld;
XMMATRIX m_view;
XMMATRIX m_proj;

float m_geoYaw = 0;
XMFLOAT3 m_geoRotation = { 0,0,0 };

Mesh* model;
Shader* shader;

void App_PreCreateWindow()
{

}

bool App_Init()
{
	if (!d3dGraphic::Init())
	{
		return false;
	}
	//
	shader = ShaderManager::LoadFromFxFile("res/effects/UnlitOpaque.fx");
	if (!shader) 
	{
		win_MsgBox(L"加载Fx文件失败！");
		return false;
	}
	//
	model = MeshManager::LoadFromTxtFile("res/models/car.txt");
	if (model == nullptr) 
	{
		win_MsgBox(L"加载模型文件失败！");
		return false;
	}
	//
	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(0, 0, 250, 1.0f);
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
	Graphics::ClearBackground(Colors::LightSteelBlue);
	Graphics::ClearDepthStencil();

	XMMATRIX mvp = m_geoWorld * m_view * m_proj;
	shader->SetMatrix4x4("gMVP", mvp);

	d3dGraphic::SetShadingMode(d3dGraphic::D3D_SHADING_MODE_WIREFRAME);

	Graphics::DrawMesh(model, shader);

	d3dGraphic::Present();
}

void App_Cleanup()
{
	MeshManager::Cleanup();
	ShaderManager::Cleanup();
	d3dGraphic::Shutdown();
}

#endif 