#include "Global.h"
#include "App.h"

#ifdef  AppImpl_IdTech4Map
#include <iostream>
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
#include "Framework/Mesh/MeshManager.h"
#include "Framework/Mesh/Mesh.h"
#include "Framework/ImGUI/ImGuiHelper.h"
#include "Framework/Scene/SceneManager.h"
#include "Framework/RenderTexture/RenderTextureManager.h"
#include "Framework/Utils/FlyingCameraEntity.h"
#include "Framework/Collision/CollisionUtils.h"
#include "Framework/Utils/GizmosHelper.h"
#include "Framework/RenderPipeline/ForwardRenderPipeline.h"
#include "Framework/RenderPipeline/ParallelLightShadowMap.h"
#include "Framework/Material/MaterialManager.h"
#include "Framework/Scene/IdTech4SceneCreator.h"

using namespace Framework;

Light* m_parallelLight;
XMFLOAT3 m_parallelLightRot = { 50,-45,0 };
float m_parallelLightIntensity = 1.0f;

RGBA32 m_bgColor = Colors::LightSteelBlue;

Camera* mainCamera;

FlyingCameraEntity* flyCameraEnt;

ForwardRenderPipeline* forwardRenderPipeline;

void DrawImGUI(Camera* renderingCamera);
void DrawGizmos(Camera* renderingCamera);

std::vector<Mesh*> m_brushMeshes;

void App_PreCreateWindow()
{
	win_SetWidth(1280);
	win_SetHeight(720);
}

void BuildScene()
{
	CreateSceneFromIdTech4MapFile("res/idtech4/maps/test_scene.proc");
	//
	mainCamera = SceneManager::CreateDefaultMainCamera();
	mainCamera->SetBackgroundColor(m_bgColor);
	//
	flyCameraEnt = new FlyingCameraEntity("FlyingCamera");
	flyCameraEnt->AttachCamera(mainCamera);
	SceneManager::AddEntity(flyCameraEnt);
	flyCameraEnt->SetEnable(true);
	//
	SceneManager::EnableLinearFog(false);
	//
}

void BuildLights()
{
	m_parallelLight = SceneManager::FindLight("MainDirectionalLight");
	if (m_parallelLight) 
	{
		m_parallelLight->EnableShadow(true);
		m_parallelLightRot = m_parallelLight->GetTransform()->rotation;
	}
}


bool App_Init()
{
	if (!d3dGraphic::Init())
	{
		return false;
	}
	//������Ⱦ����
	forwardRenderPipeline = new ForwardRenderPipeline();
	forwardRenderPipeline->SetDrawGUICallBack(DrawImGUI);
	forwardRenderPipeline->SetDrawGizmosCallBack(DrawGizmos);
	//
	ParallelLightShadowMap*  parallelLightSM = forwardRenderPipeline->GetParallelLightShadowMap();
	parallelLightSM->SetSize(2048);
	parallelLightSM->SetNearestShadowDistance(500);
	//
	SceneManager::Init(forwardRenderPipeline);
	GizmosHelper::Init();
	ImGuiHelper::Init();
	dxinput_Init(win_GetInstance(), win_GetHandle());
	//
	BuildScene();
	//
	BuildLights();
	//
	return true;
}

void App_Tick(float dt)
{
	dxinput_Tick();
	//
	if (m_parallelLight) 
	{
		m_parallelLight->GetTransform()->rotation = m_parallelLightRot;
		m_parallelLight->SetIntensity(m_parallelLightIntensity);
	}
	//
	mainCamera->SetBackgroundColor(m_bgColor);
	//
	SceneManager::Tick(dt);
}

void DrawImGUI(Camera* renderingCamera)
{
	//
	ImGuiHelper::BeginGUI();
	{
		ImGui::Begin(u8"Background And Light");
		ImGui::ColorEdit3("Background Color", (float*)&m_bgColor);
		ImGui::DragFloat3("Light Direction", (float*)&m_parallelLightRot, 1.0f);
		ImGui::SliderFloat("Light Intensity", &m_parallelLightIntensity, 0.0f, 2.0f);
		ImGui::End();
		//
		ImGui::Begin("Flying Camera", nullptr, ImGuiWindowFlags_NoCollapse);
		ImGui::SetWindowSize("Flying Camera", { 360,107 });
		ImGui::Text("Press \"W,A,S,D\" moving !");
		ImGui::Text("Press \"Left Shift\" moving faster !");
		ImGui::Text("Press \"Mouse Right Button\" looking around !");
		ImGui::Text("Press \"1\" and \"2\" switch camera !");
		ImGui::End();
	}
	ImGuiHelper::EndGUI();
}

void DrawEntityAABB(Entity* ent, Camera* renderingCamera)
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
	for (unsigned int r = 0; r < rendererCnt; ++r)
	{
		AxisAlignedBox bbox = ent->GetRenderer(r)->GetMeshInstance()->GetAxisAlignedBox();
		Transform* trans = ent->GetTransform();
		XMMATRIX scalingMat = XMMatrixScaling(bbox.Extents.x, bbox.Extents.y, bbox.Extents.z);
		XMMATRIX translateMat = XMMatrixTranslationFromFloat3(bbox.Center);
		XMMATRIX mvp = (scalingMat * translateMat) * trans->GetWorldMatrix() * renderingCamera->GetViewMatrix() * renderingCamera->GetProjectMatrix();
		RGBA32 color = CollisionUtils::IntersectRendererCamera(ent->GetRenderer(r), mainCamera) ? Colors::Green : Colors::Red;
		GizmosHelper::DrawCube(mvp, color);
	}
}


void DrawGizmos(Camera* renderingCamera)
{
	//DrawEntityAABB(terrain, renderingCamera);
	//DrawEntityAABB(actor, renderingCamera);
	//DrawEntityAABB(waterSurface, renderingCamera);
	////
	//Frustum frustumV = mainCamera->GetViewSpaceFrustum();
	//XMMATRIX mvp = mainCamera->GetTransform()->GetWorldMatrix() * renderingCamera->GetViewMatrix() * renderingCamera->GetProjectMatrix();
	//GizmosHelper::DrawFrustum(frustumV, mvp, Colors::Blue);
}

void App_Draw()
{
	SceneManager::DrawOneFrame();
}

void App_Cleanup()
{
	ReleasePointer(forwardRenderPipeline);
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

#endif //  AppImpl_IdTech4Map
