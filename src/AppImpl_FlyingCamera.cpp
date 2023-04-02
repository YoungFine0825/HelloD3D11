#include "Global.h"
#include "App.h"

#ifdef  AppImpl_FlyingCamera

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
#include "Framework/Scene/SceneManager.h"
#include "Framework/RenderTexture/RenderTextureManager.h"

#include "Framework/Utils/FlyingCameraEntity.h"

using namespace Framework;

ParallelLight* m_parallelLight;
XMFLOAT3 m_parallelLightRot = { 50,-45,0 };

RGBA32 m_bgColor = Colors::LightSteelBlue;

Entity* terrain;
Entity* actor;
float actorSpecularPower = 100.0f;

Camera* mainCamera;
Material* waterMaterial;

bool enableFog = true;
float fogStart = 500;
float fogRange = 1000;

RenderTexture* screenRT;
RenderTexture* blurRT1;
RenderTexture* blurRT2;
Shader* blurShader;
int blurCount = 0;
ID3D11ShaderResourceView* nullSRV[1] = { 0 };
ID3D11UnorderedAccessView* nullUAV[1] = { 0 };

FlyingCameraEntity* flyCameraEnt;

void App_PreCreateWindow()
{
	win_SetWidth(1280);
	win_SetHeight(720);
}

Entity* CreateEntitiesFromModel(Resource::ResourceUrl modelUrl, std::string name)
{
	Model* m = ModelManager::LoadFromObjFile(modelUrl);
	if (m == nullptr)
	{
		return nullptr;
	}
	Entity* ent = SceneManager::CreateEntity(name);
	unsigned int meshSize = m->GetMeshCount();
	for (unsigned int i = 0; i < meshSize; ++i)
	{
		Renderer* r = ent->CreateRenderer();
		r->mesh = m->GetMesh(i);
	}
	return ent;
}

void BuildScene()
{
	Shader* opaShader = ShaderManager::LoadFromFxFile("res/effects/LitOpaque.fx");
	Shader* alphaTestShader = ShaderManager::LoadFromFxFile("res/effects/UnlitAlphaTest.fx");
	Shader* transparentShader = ShaderManager::LoadFromFxFile("res/effects/LitAlpha.fx");
	//
	terrain = CreateEntitiesFromModel("res/models/terrain.obj", "terrain");
	if (terrain && terrain->GetRendererCount() > 0)
	{
		Texture* terrainTex = TextureManager::LoadDDSFromFile("res/models/grass.dds");
		Renderer* renderer = terrain->GetRenderer(0);
		renderer->material = new Material(opaShader);
		renderer->material->SetMainTexture(terrainTex);
		//
		terrain->GetTransform()->position = { 0.0f,400.0f,1181.0f };
		terrain->GetTransform()->rotation = { 0.0f,88.0f,0.0f };
		terrain->GetTransform()->scale = { 400.0f,400.0f,400.0f };
	}
	//
	actor = CreateEntitiesFromModel("res/models/FoxHowl_Teacher_BODY_BL.obj", "actor");
	if (actor != nullptr)
	{
		Renderer* renderer = actor->GetRenderer(0);
		//
		Texture* actorBodyTex = TextureManager::LoadDDSFromFile("res/models/FoxHowl_Teacher_BODY_BL.dds");
		Texture* actorHeadTex = TextureManager::LoadDDSFromFile("res/models/FoxHowl_Teacher_Face_BL.dds");
		//
		renderer->material = new Material(opaShader);
		renderer->material->SetMainTexture(actorHeadTex);
		//
		renderer = actor->GetRenderer(1);
		renderer->material = new Material(opaShader);
		renderer->material->SetMainTexture(actorBodyTex);
		//
		actor->GetTransform()->position = { 0.0f,-21.0f,300.0f };
		actor->GetTransform()->rotation = { 0.0f,0.0f,0.0f };
		actor->GetTransform()->scale = { 100.0f,100.0f,100.0f };
	}
	//
	Entity* waterQuad = SceneManager::CreateEntity("Water");
	Transform* waterTrans = waterQuad->GetTransform();
	waterTrans->position = { 0,-100,1000 };
	waterTrans->rotation = { 90,0,0 };
	waterTrans->scale = { 1500,1500,1 };
	Texture* waterTex = TextureManager::LoadDDSFromFile("res/models/water2.dds");
	Renderer* waterRenderer = waterQuad->CreateRenderer();
	waterMaterial = new Material(transparentShader);
	waterMaterial->SetMainTexture(waterTex);
	waterMaterial->mainTextureST = { 25,25,0,0 };
	waterMaterial->renderQueue = RENDER_QUEUE_TRANSPARENT;
	waterMaterial->SetFloat("obj_Alpha", 0.6f);
	waterRenderer->material = waterMaterial;
	waterRenderer->mesh = MeshManager::CreateQuad();
	//
	m_parallelLight = LightManager::GetParallelLight(0);
	m_parallelLight->intensity = 1;
	m_parallelLight->color = Colors::SunLight;
	m_parallelLight->directionW = XMVectorRotationFromFloat3({ 0,0,1 }, m_parallelLightRot);
	//
	SceneManager::EnableLinearFog(enableFog);
	SceneManager::SetLinearFogStart(fogStart);
	SceneManager::SetLinearFogRange(fogRange);
	//
	mainCamera = SceneManager::CreateCamera("MainCamera");
	mainCamera->SetAspectRatio(win_GetAspectRatio())
		->SetBackgroundColor(m_bgColor)
		->SetClearFlag(CAMERA_CLEAR_SOLID_COLOR)
		->SetNearClipDistance(1)
		->SetFarClipDistance(5000)
		->SetFov(45)
		;
	mainCamera->GetTransform()->position = { 0,0,-270 };
	//
	flyCameraEnt = new FlyingCameraEntity("FlyingCamera");
	flyCameraEnt->AttachCamera(mainCamera);
	SceneManager::AddEntity(flyCameraEnt);
}


bool App_Init()
{
	if (!d3dGraphic::Init())
	{
		return false;
	}
	//
	SceneManager::Init();
	ImGuiHelper::Init();
	dxinput_Init(win_GetInstance(), win_GetHandle());
	//
	BuildScene();
	//
	blurShader = ShaderManager::LoadFromFxFile("res/effects/ComputeShader_Blur.fx");
	//
	unsigned int width = win_GetWidth();
	unsigned int height = win_GetHeight();
	screenRT = RenderTextureManager::CreateRenderTexture(width, height);
	blurRT1 = RenderTextureManager::CreateRenderTexture(width, height, false, true);
	blurRT2 = RenderTextureManager::CreateRenderTexture(width, height, false, true);
	//
	mainCamera->SetRenderTexture(screenRT);
	//
	return true;
}

void App_Tick(float dt)
{
	dxinput_Tick();
	m_parallelLight->directionW = XMVectorRotationFromFloat3({ 0,0,1 }, m_parallelLightRot);
	//
	if (waterMaterial != nullptr)
	{
		waterMaterial->mainTextureST.z += 0.1f * dt;
	}
	//
	mainCamera->SetBackgroundColor(m_bgColor);
	//
	SceneManager::EnableLinearFog(enableFog);
	SceneManager::SetLinearFogStart(fogStart);
	SceneManager::SetLinearFogRange(fogRange);
	//
	for (size_t i = 0; i < actor->GetRendererCount(); ++i)
	{
		actor->GetRenderer(i)->material->specularPower = actorSpecularPower;
	}
	//
	SceneManager::Tick(dt);
}

void DrawImGUI()
{
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
		ImGui::DragFloat3("Position", (float*)&(actor->GetTransform()->position), 1.0f);
		ImGui::DragFloat3("Rotation", (float*)&(actor->GetTransform()->rotation), 1.0f);
		ImGui::DragFloat("Specular Power", &actorSpecularPower, 10.0f);
		ImGui::End();
		//
		ImGui::Begin(u8"Terrain Model");
		ImGui::DragFloat3("Position", (float*)&(terrain->GetTransform()->position), 1.0f);
		ImGui::DragFloat3("Rotation", (float*)&(terrain->GetTransform()->rotation), 1.0f);
		ImGui::DragFloat3("Scaling", (float*)&(terrain->GetTransform()->scale), 1.0f);
		ImGui::End();
		//
		ImGui::Begin(u8"Camera");
		ImGui::DragFloat3("Position", (float*)&(mainCamera->GetTransform()->position), 1.0f);
		ImGui::DragFloat3("Rotation", (float*)&(mainCamera->GetTransform()->rotation), 1.0f);
		ImGui::End();
		//
		ImGui::Begin(u8"Linear Fog");
		ImGui::Checkbox("Enable Fog", &enableFog);
		ImGui::DragFloat("Fog Start", &fogStart, 1.0f, 1.0f);
		ImGui::DragFloat("Fog Range", &fogRange, 5.0f, 1.0f);
		ImGui::End();
		//
		ImGui::Begin(u8"Blur");
		ImGui::DragInt("Blur Times", &blurCount, 1.0f, 0, 10);
		ImGui::End();
		//
		ImGui::Begin("Flying Camera", nullptr, ImGuiWindowFlags_NoCollapse);
		ImGui::SetWindowSize("Flying Camera", { 360,107 });
		ImGui::Text("Press \"W,A,S,D\" moving !");
		ImGui::Text("Press \"Left Shift\" moving faster !");
		ImGui::Text("Press \"Mouse Right Button\" looking around !");
		ImGui::End();
	}
	ImGuiHelper::EndGUI();
}


void DoBlur()
{
	float screenWid = screenRT->GetWidth();
	float screenHei = screenRT->GetHeight();
	Graphics::Blit(screenRT, blurRT1);
	ID3D11DeviceContext* dc = d3dGraphic::GetDeviceContext();
	for (int i = 0; i < blurCount; ++i)
	{

		blurShader->SetShaderResourceView("g_Input", blurRT1->GetColorTextureSRV());
		blurShader->SetUnorderredAccessView("g_Output", blurRT2->GetColorTextureUAV());
		blurShader->ApplyPass(0, dc);
		UINT numGroupsX = (UINT)ceilf(screenWid / 256.0f);
		dc->Dispatch(numGroupsX, screenHei, 1);

		// Unbind the input texture from the CS for good housekeeping.
		dc->CSSetShaderResources(0, 1, nullSRV);
		// Unbind output from compute shader (we are going to use this output as an input in the next pass, 
		// and a resource cannot be both an output and input at the same time.
		dc->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);


		blurShader->SetShaderResourceView("g_Input", blurRT2->GetColorTextureSRV());
		blurShader->SetUnorderredAccessView("g_Output", blurRT1->GetColorTextureUAV());
		blurShader->ApplyPass(1, dc);
		UINT numGroupsY = (UINT)ceilf(screenHei / 256.0f);
		dc->Dispatch(screenWid, numGroupsY, 1);

		dc->CSSetShaderResources(0, 1, nullSRV);
		dc->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
	}
}

void App_Draw()
{
	//d3dGraphic::EnableBackCulling(false);
	SceneManager::DrawOneFrame();
	//
	if (blurCount > 0)
	{
		DoBlur();
		//
		Graphics::Blit(blurRT1, nullptr);
	}
	else
	{
		Graphics::Blit(screenRT, nullptr);
	}
	//
	DrawImGUI();
	//
	Graphics::Present();
}

void App_Cleanup()
{
	ImGuiHelper::Cleanup();
	SceneManager::Cleanup();
	ModelManager::Cleanup();
	MeshManager::Cleanup();
	ShaderManager::Cleanup();
	TextureManager::Cleanup();
	RenderTextureManager::Cleanup();
	dxinput_Cleanup();
	d3dGraphic::Shutdown();
}

#endif //  AppImpl_FlyingCamera
