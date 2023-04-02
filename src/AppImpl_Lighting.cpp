
#include "Global.h"
#include "App.h"

#ifdef AppImpl_Lighting

#include "d3d/DXInput.h"
#include "Window.h"
#include "math/MathLib.h"
#include "Color.h"

#include "Framework/Graphic.h"
#include "Framework/Mesh/MeshManager.h"
#include "Framework/Shader/ShaderManager.h"

#include <vector>;

using namespace Framework;


XMMATRIX m_view;
XMMATRIX m_proj;
XMFLOAT3 m_cameraWorldPos = { 0, 5, -20 };

Shader* shader;

struct ParallelLight
{
	XMFLOAT4 color;
	XMFLOAT3 directionW;
	float intensity;
};

struct PointLight
{
	XMFLOAT4 color;
	XMFLOAT3 positionW;
	float intensity;
	XMFLOAT3 attenuation;
	float range;
};

struct SpotLight
{
	XMFLOAT4 color;
	XMFLOAT3 positionW;
	float intensity;
	XMFLOAT3 attenuation;
	float range;
	XMFLOAT3 directionW;
	float spot;
};

ParallelLight g_parallelLight;
#define POINT_LIGHT_COUNT 6
PointLight g_pointLights[POINT_LIGHT_COUNT];
#define SPOT_LIGHT_COUNT 6
SpotLight g_spotLights[SPOT_LIGHT_COUNT];

struct Renderable
{
	Mesh* meshPtr;
	XMFLOAT3 positionW = {0,0,0};
	XMFLOAT3 rotationW = {0,0,0};
	XMFLOAT3 scaleL = {1,1,1};
	RGBA32 ambientColor = { 0.2f,0.2f,0.2f,1.0f };
	RGBA32 diffuseColor = { 0.6f,0.0f,0.0f,1.0f };
	RGBA32 specularColor = Colors::White;
	float specularPower = 200;
};

std::vector<Renderable> renderableEnities;

void App_PreCreateWindow()
{

}

void Init_Entites() 
{
	Renderable model;
	model.meshPtr = MeshManager::LoadFromTxtFile("res/models/car.txt");
	model.rotationW = { 0,90,0 };
	model.ambientColor = { 0.2f,0.2f,0.2f,1.0f };
	model.diffuseColor =  Colors::Silver;
	model.specularColor = Colors::Silver;
	renderableEnities.push_back(model);

	Renderable quad;
	quad.meshPtr = MeshManager::CreateQuad();
	quad.positionW = { 0,-5,0 };
	quad.rotationW = { 90,0,0 };
	quad.scaleL = { 30,30,30 };
	quad.ambientColor = Colors::Black;
	quad.diffuseColor = Colors::White;
	quad.specularColor = Colors::White;
	quad.specularPower = 300;
	renderableEnities.push_back(quad);

	Renderable wall;
	wall.meshPtr = MeshManager::CreateQuad();
	wall.positionW = { 0,0,30 };
	wall.rotationW = { 0,0,0 };
	wall.scaleL = { 30,30,30 };
	wall.ambientColor = Colors::Black;
	wall.diffuseColor = Colors::White;
	wall.specularColor = Colors::White;
	wall.specularPower = 300;
	renderableEnities.push_back(wall);
}

void Init_Lights() 
{
	//
	ZeroMemory(&g_parallelLight, sizeof(g_parallelLight));
	g_parallelLight.intensity = 1;
	g_parallelLight.color = Colors::SunLight;
	g_parallelLight.directionW = { -1,-1,0 };
	//
	for (int i = 0; i < POINT_LIGHT_COUNT; ++i)
	{
		ZeroMemory(&g_pointLights[i], sizeof(g_pointLights[i]));
		g_pointLights[i].attenuation = { 1,1,1 };
		g_pointLights[i].intensity = 0;
		g_pointLights[i].range = 1.0f;
	}
	g_pointLights[0].color = Colors::Green;
	g_pointLights[0].intensity = 1.0f;
	g_pointLights[0].positionW = { 10,2,0 };
	g_pointLights[0].range = 10.0f;
	//
	for (int i = 0; i < SPOT_LIGHT_COUNT; ++i)
	{
		ZeroMemory(&g_spotLights[i], sizeof(g_spotLights[i]));
		g_spotLights[i].attenuation = { 1,1,1 };
		g_spotLights[i].intensity = 0;
		g_spotLights[i].range = 1.0f;
	}
	//
	g_spotLights[0].color = Colors::Blue;
	g_spotLights[0].intensity = 1.0f;
	g_spotLights[0].positionW = { -13,0.2f,-30 };
	g_spotLights[0].range = 100.0f;
	g_spotLights[0].spot = Angle2Radin(20);
	g_spotLights[0].directionW = {0,0,1};
}

bool App_Init()
{
	
	if (!d3dGraphic::Init())
	{
		return false;
	}
	//
	dxinput_Init(win_GetInstance(),win_GetHandle());
	//
	shader = ShaderManager::LoadFromFxFile("res/effects/BlinnPhongOpaque.fx");
	if (!shader)
	{
		win_MsgBox(L"¼ÓÔØFxÎÄ¼þÊ§°Ü£¡");
		return false;
	}
	//
	// Build the view matrix.
	m_view = XMMatrixLookAtFromFloat3LH(m_cameraWorldPos, { 0,0,0 }, {0,1.0f,0});
	//
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	m_proj = XMMatrixPerspectiveFovLH(0.25f * PI, win_GetAspectRatio(), 1.0f, 1000.0f);
	//
	Init_Lights();
	//
	Init_Entites();
	//
	return true;
}

void App_Tick(float dt)
{
	dxinput_Tick();

	if (dxinput_IsKeyDown(DIK_LEFTARROW)) 
	{
		g_pointLights[0].positionW.x -= 10 * dt;
	}
	else if (dxinput_IsKeyDown(DIK_RIGHTARROW)) 
	{
		g_pointLights[0].positionW.x += 10 * dt;
	}
	else if (dxinput_IsKeyDown(DIK_UPARROW))
	{
		g_pointLights[0].positionW.z += 10 * dt;
	}
	else if (dxinput_IsKeyDown(DIK_DOWNARROW))
	{
		g_pointLights[0].positionW.z -= 10 * dt;
	}

	if (dxinput_IsKeyDown(DIK_A))
	{
		g_spotLights[0].positionW.x -= 10 * dt;
	}
	else if (dxinput_IsKeyDown(DIK_D))
	{
		g_spotLights[0].positionW.x += 10 * dt;
	}
	else if (dxinput_IsKeyDown(DIK_W))
	{
		g_spotLights[0].positionW.z += 10 * dt;
	}
	else if (dxinput_IsKeyDown(DIK_S))
	{
		g_spotLights[0].positionW.z -= 10 * dt;
	}

	renderableEnities[0].rotationW.y += 10 * dt;
}

void App_Draw()
{
	Graphics::ClearBackground(Colors::Black);
	Graphics::ClearDepthStencil();


	for (size_t i = 0; i < renderableEnities.size(); ++i) 
	{
		Renderable ent = renderableEnities[i];
		if (ent.meshPtr == nullptr) 
		{
			continue;
		}
		//Camera
		shader->SetVector3("g_CameraPosW", m_cameraWorldPos);
		shader->SetMatrix4x4("obj_MatView", m_view);
		shader->SetMatrix4x4("obj_MatProj", m_proj);
		//Lights
		shader->SetStruct("g_ParallelLight", &g_parallelLight, sizeof(g_parallelLight));
		shader->SetStruct("g_PointLights", &g_pointLights, sizeof(g_pointLights));
		shader->SetStruct("g_SpotLights", &g_spotLights, sizeof(g_spotLights));
		//Material
		shader->SetVector4("obj_AmbientColor", ent.ambientColor);
		shader->SetVector4("obj_DiffuseColor", ent.diffuseColor);
		shader->SetVector4("obj_SpecularColor", ent.specularColor);
		shader->SetFloat("obj_SpecularPower", ent.specularPower);
		//Transform
		XMMATRIX matWorld = XMMatrixScaling(ent.scaleL.x, ent.scaleL.y, ent.scaleL.z)
			* XMMatrixRotationFromFloat3(ent.rotationW)
			* XMMatrixTranslation(ent.positionW.x, ent.positionW.y, ent.positionW.z);
		shader->SetMatrix4x4("obj_MatWorld", matWorld);
		XMMATRIX normalWorldMat = XMMatrixInverseTranspose(matWorld);
		shader->SetMatrix4x4("obj_MatNormalWorld", normalWorldMat);
		XMMATRIX mvp = matWorld * m_view * m_proj;
		shader->SetMatrix4x4("obj_MatMVP", mvp);
		//
		Graphics::DrawMesh(ent.meshPtr, shader);
	}

	d3dGraphic::Present();
}

void App_Cleanup()
{
	MeshManager::Cleanup();
	ShaderManager::Cleanup();
	dxinput_Cleanup();
	d3dGraphic::Shutdown();
	renderableEnities.clear();
}

#endif 