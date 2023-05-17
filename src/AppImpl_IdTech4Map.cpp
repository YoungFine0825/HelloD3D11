#include "Global.h"
#include "App.h"

#ifdef  AppImpl_IdTech4Map
#include <iostream>
#include "d3d/DXInput.h"
#include "Window.h"
#include "math/MathLib.h"
#include "Color.h"

#include "Framework/Root.h"
//
#include "Framework/Utils/FlyingCameraEntity.h"
//
#include "Framework/RenderPipeline/ForwardRenderPipeline.h"
#include "Framework/RenderPipeline/ParallelLightShadowMap.h"
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

void App_PreCreateWindow()
{
	win_SetWidth(1280);
	win_SetHeight(720);
}


bool App_Init()
{
	if (!Framework::Root::Init())
	{
		return false;
	}
	//创建渲染管线
	forwardRenderPipeline = new ForwardRenderPipeline();
	forwardRenderPipeline->SetDrawGUICallBack(DrawImGUI);
	forwardRenderPipeline->SetDrawGizmosCallBack(DrawGizmos);
	//
	ParallelLightShadowMap*  parallelLightSM = forwardRenderPipeline->GetParallelLightShadowMap();
	parallelLightSM->SetSize(2048);
	parallelLightSM->SetNearestShadowDistance(500);
	//
	Framework::RenderSystem::SetRenderPipeline(forwardRenderPipeline);
	//
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
	m_parallelLight = SceneManager::FindLight("MainDirectionalLight");
	if (m_parallelLight)
	{
		m_parallelLight->EnableShadow(true);
		m_parallelLightRot = m_parallelLight->GetTransform()->rotation;
	}
	//
	return true;
}

void App_Tick(float dt)
{
	//
	if (m_parallelLight) 
	{
		m_parallelLight->GetTransform()->rotation = m_parallelLightRot;
		m_parallelLight->SetIntensity(m_parallelLightIntensity);
	}
	//
	if (mainCamera) 
	{
		mainCamera->SetBackgroundColor(m_bgColor);
	}
	//
	Framework::Root::Tick(dt);
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

void DrawGizmos(Camera* renderingCamera)
{

}

void App_Draw()
{
	Framework::Root::Draw();
}

void App_Cleanup()
{
	ReleasePointer(forwardRenderPipeline);
	Framework::Root::Shutdown();
}

#endif //  AppImpl_IdTech4Map
