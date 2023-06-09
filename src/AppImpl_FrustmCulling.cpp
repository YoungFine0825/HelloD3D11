#include "Global.h"
#include "App.h"

#ifdef  AppImpl_FrustmCulling

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
#include "Framework/Collision/CollisionUtils.h"
#include "Framework/Utils/GizmosHelper.h"

using namespace Framework;

ParallelLight* m_parallelLight;
XMFLOAT3 m_parallelLightRot = { 50,-45,0 };

RGBA32 m_bgColor = Colors::LightSteelBlue;

Entity* terrain;
Entity* actor;
float actorSpecularPower = 100.0f;

Camera* mainCamera;
Camera* secondCamera;
Material* waterMaterial;

FlyingCameraEntity* flyCameraEnt;
FlyingCameraEntity* flyCameraEnt2;
int curCameraIndex = 1;

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
	mainCamera = SceneManager::CreateCamera("MainCamera");
	mainCamera->SetAspectRatio(win_GetAspectRatio())
		->SetBackgroundColor(m_bgColor)
		->SetClearFlag(CAMERA_CLEAR_SOLID_COLOR)
		->SetNearClipDistance(1.0f)
		->SetFarClipDistance(5000)
		->SetFov(45)
		;
	mainCamera->GetTransform()->position = { 0,0,-270 };
	//
	secondCamera = SceneManager::CreateCamera("SecondCamera");
	secondCamera->SetAspectRatio(win_GetAspectRatio())
		->SetBackgroundColor(m_bgColor)
		->SetClearFlag(CAMERA_CLEAR_SOLID_COLOR)
		->SetNearClipDistance(1)
		->SetFarClipDistance(5000)
		->SetFov(45)
		;
	secondCamera->GetTransform()->position = { 1436,883,-832 };
	secondCamera->GetTransform()->rotation = { 20,-46,0 };
	//
	flyCameraEnt = new FlyingCameraEntity("FlyingCamera");
	flyCameraEnt->AttachCamera(mainCamera);
	SceneManager::AddEntity(flyCameraEnt);
	flyCameraEnt->SetEnable(true);
	//
	flyCameraEnt2 = new FlyingCameraEntity("FlyingCamera2");
	flyCameraEnt2->AttachCamera(secondCamera);
	SceneManager::AddEntity(flyCameraEnt2);
	flyCameraEnt2->SetEnable(false);
	//
	SceneManager::SetLinearFogStart(1000);
	SceneManager::SetLinearFogRange(1000);
}


bool App_Init()
{
	if (!d3dGraphic::Init())
	{
		return false;
	}
	//
	SceneManager::Init();
	GizmosHelper::Init();
	ImGuiHelper::Init();
	dxinput_Init(win_GetInstance(), win_GetHandle());
	//
	BuildScene();
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
		waterMaterial->mainTextureST.z += 0.2f * dt;
	}
	//
	mainCamera->SetBackgroundColor(m_bgColor);
	//
	for (size_t i = 0; i < actor->GetRendererCount(); ++i)
	{
		actor->GetRenderer(i)->material->specularPower = actorSpecularPower;
	}
	//
	SceneManager::Tick(dt);
	//
	if (dxinput_IsKeyDown(DIK_1)) 
	{
		curCameraIndex = 1;
	}
	if (dxinput_IsKeyDown(DIK_2)) 
	{
		curCameraIndex = 2;
	}
	mainCamera->SetEnable(curCameraIndex == 1);
	//flyCameraEnt->SetEnable(curCameraIndex == 1);
	secondCamera->SetEnable(curCameraIndex == 2);
	//flyCameraEnt2->SetEnable(curCameraIndex == 2);
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
		ImGui::Begin("Flying Camera", nullptr, ImGuiWindowFlags_NoCollapse);
		ImGui::SetWindowSize("Flying Camera", { 360,107 });
		ImGui::Text("Press \"W,A,S,D\" moving !");
		ImGui::Text("Press \"Left Shift\" moving faster !");
		ImGui::Text("Press \"Mouse Right Button\" looking around !");
		ImGui::Text("Press \"1\" and \"2\" switch camera !");
		//ImGui::InputFloat3("SecondCameraPos", (float*)(&secondCamera->GetTransform()->position));
		//ImGui::InputFloat3("SecondCameraRot", (float*)(&secondCamera->GetTransform()->rotation));
		ImGui::End();
	}
	ImGuiHelper::EndGUI();
}

void DrawEntityAABB(Entity* ent) 
{
	if (!ent) 
	{
		return;
	}
	unsigned int rendererCnt = ent->GetRendererCount();
	if (rendererCnt <= 0)
	{
		return;
	}
	Camera* renderingCamera = curCameraIndex == 1 ? mainCamera : secondCamera;
	for (unsigned int r = 0; r < rendererCnt; ++r) 
	{
		AxisAlignedBox bbox = ent->GetRenderer(r)->mesh->GetAxisAlignedBox();
		Transform* trans = ent->GetTransform();
		XMMATRIX scalingMat = XMMatrixScaling(bbox.Extents.x, bbox.Extents.y, bbox.Extents.z);
		XMMATRIX translateMat = XMMatrixTranslationFromFloat3(bbox.Center);
		XMMATRIX mvp = (scalingMat * translateMat) * trans->GetWorldMatrix() * renderingCamera->GetViewMatrix() * renderingCamera->GetProjectMatrix();
		RGBA32 color = CollisionUtils::IntersectRendererCamera(ent->GetRenderer(r), mainCamera) ? Colors::Green : Colors::Red;
		GizmosHelper::DrawCube(mvp, color);
	}
}

void DrawGizmos()
{
	DrawEntityAABB(terrain);
	DrawEntityAABB(actor);
	DrawEntityAABB(SceneManager::FindEntity("Water"));
	//
	Camera* renderingCamera = curCameraIndex == 1 ? mainCamera : secondCamera;
	Frustum frustumV = mainCamera->GetViewSpaceFrustum();
	XMMATRIX mvp = mainCamera->GetTransform()->GetWorldMatrix() * renderingCamera->GetViewMatrix() * renderingCamera->GetProjectMatrix();
	GizmosHelper::DrawFrustum(frustumV, mvp, Colors::Blue);

}

void App_Draw()
{
	SceneManager::DrawOneFrame();
	//
	DrawGizmos();
	//
	DrawImGUI();
	//
	Graphics::Present();
}

void App_Cleanup()
{
	GizmosHelper::Cleanup();
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

#endif //  AppImpl_FrustmCulling
