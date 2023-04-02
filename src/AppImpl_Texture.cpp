#include "Global.h"
#include "App.h"

#ifdef AppImpl_Texture

#include "d3d/DXInput.h"
#include "Window.h"
#include "math/MathLib.h"
#include "Color.h"

#include "Framework/Graphic.h"
#include "Framework/Model/ModelManager.h"
#include "Framework/Model/Model.h"
#include "Framework/Shader/ShaderManager.h"
#include "Framework/Texture/Texture.h"
#include "Framework/Texture/TextureManager.h"
#include "Framework/Material/Material.h"
#include "Framework/Light/LightManager.h"
#include "Framework/Mesh/MeshManager.h"
#include "Framework/Mesh/Mesh.h"
#include "Framework/ImGUI/ImGuiHelper.h"

using namespace Framework;

struct Transform 
{
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 scale = { 1,1,1 };
	XMMATRIX GetWorldMatrix() 
	{
		XMMATRIX trans = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX rot = XMMatrixRotationFromFloat3(rotation);
		XMMATRIX scaling = XMMatrixScaling(scale.x, scale.y, scale.z);
		return scaling * rot * trans;
	}
};

RGBA32 m_bgColor = Colors::LightSteelBlue;

XMMATRIX m_view;
XMMATRIX m_proj;

Model* model;
Transform modelTrans;
SimpleMaterial m_modelMat;
Texture* m_diffuseMapBody;
Texture* m_diffuseMapHead;

Mesh* quad;
Transform quadTrans;
SimpleMaterial quadMat;
Texture* quadDiffuseMap;

Shader* shader;

ParallelLight* m_parallelLight;
XMFLOAT3 m_parallelLightRot = { 0,-45,0 };

float waterFluidSpeed = 0.1f;

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
	shader = ShaderManager::LoadFromFxFile("res/effects/LitOpaque.fx");
	if (!shader)
	{
		win_MsgBox(L"加载Fx文件失败！");
		return false;
	}
	//
	model = ModelManager::LoadFromObjFile("res/models/FoxHowl_Teacher_BODY_BL.obj");
	if (model == nullptr)
	{
		win_MsgBox(L"加载模型文件失败！");
		return false;
	}
	//
	ImGuiHelper::Init();
	dxinput_Init(win_GetInstance(), win_GetHandle());
	//
	m_diffuseMapBody = TextureManager::LoadDDSFromFile("res/models/FoxHowl_Teacher_BODY_BL.dds");
	if (m_diffuseMapBody == nullptr)
	{
		win_MsgBox(L"加载纹理文件失败！");
	}
	m_diffuseMapHead = TextureManager::LoadDDSFromFile("res/models/FoxHowl_Teacher_Face_BL.dds");
	if (m_diffuseMapHead == nullptr)
	{
		win_MsgBox(L"加载纹理文件失败！");
	}
	//
	//cube = MeshManager::LoadFromObjFile("res/models/cube.obj");
	quad = MeshManager::CreateQuad();
	quadDiffuseMap = TextureManager::LoadDDSFromFile("res/models/water2.dds");
	//
	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(0, 0, -270, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_view = XMMatrixLookAtLH(pos, target, up);
	//
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	m_proj = XMMatrixPerspectiveFovLH(0.25f * PI, win_GetAspectRatio(), 1.0f, 1000.0f);
	//
	m_parallelLight = LightManager::GetParallelLight(0);
	m_parallelLight->intensity = 1;
	m_parallelLight->color = Colors::SunLight;
	m_parallelLight->directionW = XMVectorRotationFromFloat3({0,0,1},m_parallelLightRot);
	//
	modelTrans.position.y = 10;
	//
	quadTrans.position.y = -100;
	quadTrans.position.z = 150;
	quadTrans.rotation.x = 90;
	quadTrans.scale = { 200,200,1 };
	quadMat.diffuseMapST.x = 5;
	quadMat.diffuseMapST.y = 5;
	//
	return true;
}

void App_Tick(float dt)
{
	dxinput_Tick();
	quadMat.diffuseMapST.z += waterFluidSpeed * dt;
}

void SetTransformShaderParamters(Shader* shader, Transform trans) 
{
	XMMATRIX worldMat = trans.GetWorldMatrix();
	shader->SetMatrix4x4("obj_MatWorld", worldMat);
	XMMATRIX normalWorldMat = XMMatrixInverseTranspose(worldMat);
	shader->SetMatrix4x4("obj_MatNormalWorld", normalWorldMat);
	XMMATRIX mvp = worldMat * m_view * m_proj;
	shader->SetMatrix4x4("obj_MatMVP", mvp);
}

void DrawModel(Model* model,Shader* shader) 
{
	SetTransformShaderParamters(shader, modelTrans);
	//
	shader->SetStruct("obj_Material", &m_modelMat, sizeof(m_modelMat));
	//
	shader->SetTexture("g_diffuseMap", m_diffuseMapBody);
	Graphics::DrawMesh(model->GetMesh(0), shader);
	//
	shader->SetTexture("g_diffuseMap", m_diffuseMapHead);
	Graphics::DrawMesh(model->GetMesh(1), shader);
}

void App_Draw()
{
	Graphics::ClearBackground(m_bgColor);
	Graphics::ClearDepthStencil();
	//
	m_parallelLight->directionW = XMVectorRotationFromFloat3({ 0,0,1 }, m_parallelLightRot);
	LightManager::PackLightParamtersToShader(shader);
	//
	DrawModel(model,shader);
	//
	SetTransformShaderParamters(shader, quadTrans);
	shader->SetStruct("obj_Material", &quadMat, sizeof(quadMat));
	shader->SetTexture("g_diffuseMap", quadDiffuseMap);
	Graphics::DrawMesh(quad, shader);
	//
	ImGuiHelper::BeginGUI();
	{
		ImGui::Begin(u8"Control");
		ImGui::ColorEdit3("Background Color", (float*)&m_bgColor);
		ImGui::DragFloat3("Light Direction", (float*)&m_parallelLightRot, 1.0f);
		ImGui::SliderFloat("Light Intensity", &m_parallelLight->intensity, 0.0f, 1.0f);
		ImGui::DragFloat3("Model Position", (float*)&modelTrans.position, 1.0f);
		ImGui::DragFloat3("Model Rotation", (float*)&modelTrans.rotation, 1.0f);
		ImGui::ColorEdit3("Speacular Color", (float*)&m_modelMat.specularColor);
		ImGui::DragFloat("Speacular Power", &m_modelMat.specularColor.w, 5.0f,50.0f,800.0f);
		ImGui::DragFloat3("Water Scaling", (float*)&quadTrans.scale, 1.0f,1.0f);
		ImGui::DragFloat("Water Fluid Speed", &waterFluidSpeed, 0.01f, 0.1f);
		ImGui::End();
	}
	ImGuiHelper::EndGUI();
	//
	d3dGraphic::Present();
}

void App_Cleanup()
{
	ReleaseCOM(m_diffuseMapBody);
	ReleaseCOM(m_diffuseMapHead);
	ModelManager::Cleanup();
	ShaderManager::Cleanup();
	TextureManager::Cleanup();
	dxinput_Cleanup();
	ImGuiHelper::Cleanup();
	d3dGraphic::Shutdown();
}

#endif