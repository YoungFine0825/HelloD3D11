#include "Global.h"
#include "App.h"

#ifdef AppImpl_Blending

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

#include <vector>

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

struct Entity
{
	Transform transform;
	Model* model = nullptr;
	Mesh* mesh = nullptr;
	SimpleMaterial material;
	Shader* shader = nullptr;
	std::vector<Texture*> textures;
};

std::vector<Entity> m_opaqueEntites;
std::vector<Entity> m_alphaTestEntites;
std::vector<Entity> m_transparentEntites;

RGBA32 m_bgColor = Colors::LightSteelBlue;

XMFLOAT3 m_cameraPos = { 0, 0, -270 };
XMMATRIX m_view;
XMMATRIX m_proj;

ParallelLight* m_parallelLight;
XMFLOAT3 m_parallelLightRot = { 50,-45,0 };

bool m_enableBlending = true;
ID3D11BlendState* m_blendingEnabled;
float m_alpha = 0.6f;
float m_clipOff = 0.1f;

bool m_enableFog = true;
RGBA32 m_linearFogColor = { 0.5f,0.5f,0.5f,1.0f };
float m_linearFogStart = 100;
float m_linearFogRange = 2000;

void App_PreCreateWindow()
{

}

void InitEntities() 
{
	////////////////////////////不透明物体/////////////////////////////////
	Shader* litOpaShader = ShaderManager::LoadFromFxFile("res/effects/LitOpaque.fx");
	//
	Entity actor;
	actor.mesh = nullptr;
	actor.model = ModelManager::LoadFromObjFile("res/models/FoxHowl_Teacher_BODY_BL.obj");
	Texture* actorBodyTex = TextureManager::LoadDDSFromFile("res/models/FoxHowl_Teacher_BODY_BL.dds");
	Texture* actorHeadTex = TextureManager::LoadDDSFromFile("res/models/FoxHowl_Teacher_Face_BL.dds");
	actor.shader = litOpaShader;
	actor.textures.push_back(actorBodyTex);
	actor.textures.push_back(actorHeadTex);
	actor.transform.position = {0.0f,-21.0f,60.0f};
	actor.transform.rotation = { 0.0f,-153.0f,0.0f };
	m_opaqueEntites.push_back(actor);
	//
	Entity terrain;
	terrain.mesh = nullptr;
	terrain.model = ModelManager::LoadFromObjFile("res/models/terrain.obj");
	Texture* terrainTex = TextureManager::LoadDDSFromFile("res/models/grass.dds");
	terrain.textures.push_back(terrainTex);
	terrain.shader = litOpaShader;
	terrain.transform.position = { 0.0f,400.0f,1181.0f };
	terrain.transform.rotation = { 0.0f,88.0f,0.0f };
	terrain.transform.scale = { 400.0f,400.0f,400.0f };
	m_opaqueEntites.push_back(terrain);
	//
	//
	////////////////////////////AlphaTest物体/////////////////////////////////
	Shader* alphaTestShader = ShaderManager::LoadFromFxFile("res/effects/UnlitAlphaTest.fx");
	//
	Entity fence;
	fence.model = nullptr;
	fence.mesh = MeshManager::CreateQuad();
	Texture* fenceTex = TextureManager::LoadDDSFromFile("res/textures/WireFence.dds");
	fence.shader = alphaTestShader;
	fence.textures.push_back(fenceTex);
	fence.transform.position = { -80.0f,-40.0f,20.0f };
	fence.transform.scale = { 80.0f,80.0f,1.0f };
	fence.material.diffuseMapST = { 5.0f,5.0f,0.0f,0.0f };
	m_alphaTestEntites.push_back(fence);
	//
	////////////////////////////透明物体/////////////////////////////////
	Shader* transparentShader = ShaderManager::LoadFromFxFile("res/effects/UnlitAlpha.fx");
	//
	Entity water;
	water.mesh = MeshManager::CreateQuad();
	water.model = nullptr;
	water.shader = transparentShader;
	Texture* waterTex = TextureManager::LoadDDSFromFile("res/models/water2.dds");
	water.textures.push_back(waterTex);
	water.transform.position.y = -100;
	water.transform.position.z = 150;
	water.transform.rotation.x = 90;
	water.transform.scale = { 500,500,1 };
	water.material.diffuseMapST.x = 5;
	water.material.diffuseMapST.y = 5;
	m_transparentEntites.push_back(water);
}

void InitBlendingState() 
{
	D3D11_BLEND_DESC desc = { 0 };
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;
	D3D11_RENDER_TARGET_BLEND_DESC* rt = &desc.RenderTarget[0];
	rt->BlendEnable = true;
	rt->SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rt->DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rt->BlendOp = D3D11_BLEND_OP_ADD;
	rt->SrcBlendAlpha = D3D11_BLEND_ONE;
	rt->DestBlendAlpha = D3D11_BLEND_ZERO;
	rt->BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rt->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	d3dGraphic::GetDevice()->CreateBlendState(&desc, &m_blendingEnabled);
}

bool App_Init()
{
	if (!d3dGraphic::Init())
	{
		return false;
	}
	//
	ImGuiHelper::Init();
	dxinput_Init(win_GetInstance(), win_GetHandle());
	//
	// Build the view matrix.
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_view = XMMatrixLookAtLH(XMVectorSet(m_cameraPos), target, up);
	//
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	m_proj = XMMatrixPerspectiveFovLH(0.25f * PI, win_GetAspectRatio(), 1.0f, 5000.0f);
	//
	m_parallelLight = LightManager::GetParallelLight(0);
	m_parallelLight->intensity = 1;
	m_parallelLight->color = Colors::SunLight;
	m_parallelLight->directionW = XMVectorRotationFromFloat3({ 0,0,1 }, m_parallelLightRot);
	//
	InitEntities();
	//
	InitBlendingState();
	//
	return true;
}

void App_Tick(float dt)
{
	dxinput_Tick();

	m_parallelLight->directionW = XMVectorRotationFromFloat3({ 0,0,1 }, m_parallelLightRot);

	if (m_transparentEntites.size() > 0) 
	{
		m_transparentEntites[0].material.diffuseMapST.z += 0.1f * dt;
	}
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

void DrawEntity(Entity* ent) 
{
	if (ent->model == nullptr && ent->mesh == nullptr) 
	{
		return;
	}
	if (ent->shader == nullptr) 
	{
		return;
	}
	//
	ent->shader->SetVector3("g_CameraPosW", m_cameraPos);
	//
	if(m_enableFog)
	{
		ent->shader->SetEnabledTechnique("UseLinearFog");
	}
	else 
	{
		ent->shader->SetEnabledTechnique("Default");
	}
	ent->shader->SetVector4("g_linearFogColor", m_linearFogColor);
	ent->shader->SetFloat("g_linearFogStart", m_linearFogStart);
	ent->shader->SetFloat("g_linearFogRange", m_linearFogRange);
	//
	LightManager::PackLightParamtersToShader(ent->shader);
	//
	SetTransformShaderParamters(ent->shader, ent->transform);
	//
	ent->shader->SetStruct("obj_Material", &ent->material, sizeof(ent->material));
	//
	if (ent->model != nullptr)
	{
		unsigned int meshCnt = ent->model->GetMeshCount();
		for (unsigned int m = 0; m < meshCnt; ++m) 
		{
			ent->shader->SetTexture("g_diffuseMap", ent->textures[m]);
			//
			Graphics::DrawMesh(ent->model->GetMesh(m), ent->shader);
		}
	}
	else if(ent->mesh != nullptr)
	{
		ent->shader->SetTexture("g_diffuseMap", ent->textures[0]);
		//
		Graphics::DrawMesh(ent->mesh, ent->shader);
	}
}

void App_Draw()
{
	Graphics::ClearBackground(m_bgColor);
	Graphics::ClearDepthStencil();
	//
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	//
	//先画不透明物体
	for (size_t i = 0; i < m_opaqueEntites.size(); ++i)
	{
		DrawEntity(&m_opaqueEntites[i]);
	}
	//
	//然后画AlphaTest物体
	for (size_t i = 0; i < m_alphaTestEntites.size(); ++i)
	{
		if (m_alphaTestEntites[i].shader)
		{
			m_alphaTestEntites[i].shader->SetFloat("obj_ClipOff", m_clipOff);
		}
		DrawEntity(&m_alphaTestEntites[i]);
	}
	//
	if (m_enableBlending) 
	{
		d3dGraphic::GetDeviceContext()->OMSetBlendState(m_blendingEnabled, blendFactor, 0xffffffff);
	}
	//再画透明物体
	for (size_t i = 0; i < m_transparentEntites.size(); ++i)
	{
		if (m_transparentEntites[i].shader) 
		{
			m_transparentEntites[i].shader->SetFloat("obj_Alpha", m_alpha);
		}
		DrawEntity(&m_transparentEntites[i]);
	}
	//
	d3dGraphic::GetDeviceContext()->OMSetBlendState(0, blendFactor, 0xffffffff);
	//
	ImGuiHelper::BeginGUI();
	{
		ImGui::Begin(u8"Background And Light");
		ImGui::ColorEdit3("Background Color", (float*)&m_bgColor);
		ImGui::DragFloat3("Light Direction", (float*)&m_parallelLightRot, 1.0f);
		ImGui::SliderFloat("Light Intensity", &m_parallelLight->intensity, 0.0f, 1.0f);
		ImGui::End();
		//
		ImGui::Begin(u8"Charactor Model");
		ImGui::DragFloat3("Position", (float*)&m_opaqueEntites[0].transform.position, 1.0f);
		ImGui::DragFloat3("Rotation", (float*)&m_opaqueEntites[0].transform.rotation, 1.0f);
		ImGui::End();
		//
		ImGui::Begin(u8"Terrain Model");
		ImGui::DragFloat3("Position", (float*)&m_opaqueEntites[1].transform.position, 1.0f);
		ImGui::DragFloat3("Rotation", (float*)&m_opaqueEntites[1].transform.rotation, 1.0f);
		ImGui::DragFloat3("Scaling", (float*)&m_opaqueEntites[1].transform.scale, 1.0f);
		ImGui::End();
		//
		ImGui::Begin(u8"Blending Control");
		ImGui::Checkbox("Enable Blending", &m_enableBlending);
		ImGui::SliderFloat("Alpha", &m_alpha,0.0f,1.0f);
		ImGui::End();
		//
		ImGui::Begin(u8"Fence");
		ImGui::DragFloat3("Position", (float*)&m_alphaTestEntites[0].transform.position, 1.0f);
		ImGui::DragFloat3("Rotation", (float*)&m_alphaTestEntites[0].transform.rotation, 1.0f);
		ImGui::DragFloat3("Scaling", (float*)&m_alphaTestEntites[0].transform.scale, 1.0f);
		ImGui::SliderFloat("Clip Off", &m_clipOff, 0.0f, 1.0f);
		ImGui::InputFloat2("Texture Tilling", (float*)&m_alphaTestEntites[0].material.diffuseMapST);
		ImGui::End();
		//
		ImGui::Begin(u8"Linear Fog");
		ImGui::Checkbox("Enable Fog", &m_enableFog);
		ImGui::ColorEdit3("Fog Color", (float*)&m_linearFogColor);
		ImGui::DragFloat("Fog Start", &m_linearFogStart, 1.0f, 1.0f);
		ImGui::DragFloat("Fog Range", &m_linearFogRange, 5.0f, 1.0f);
		ImGui::End();
	}
	ImGuiHelper::EndGUI();
	//
	d3dGraphic::Present();
}

void App_Cleanup()
{
	ReleaseCOM(m_blendingEnabled);
	m_opaqueEntites.clear();
	m_alphaTestEntites.clear();
	m_transparentEntites.clear();
	ModelManager::Cleanup();
	MeshManager::Cleanup();
	ShaderManager::Cleanup();
	TextureManager::Cleanup();
	dxinput_Cleanup();
	ImGuiHelper::Cleanup();
	d3dGraphic::Shutdown();
}

#endif