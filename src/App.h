#pragma once

//#define AppImpl_Empty
//#define AppImpl_HelloWorld
//#define AppImpl_LoadMeshFromFile
//#define AppImpl_Lighting
//#define AppImpl_Texture
//#define AppImpl_Blending
//#define AppImpl_SceneManager
//#define  AppImpl_ComputeShader1
//#define  AppImpl_ComputeShaderBlur
//#define AppImpl_FlyingCamera
//#define  AppImpl_FrustmCulling
#define AppImpl_ForwardRenderPipeline

void App_PreCreateWindow();
bool App_Init();
void App_Tick(float dt);
void App_Draw();
void App_Cleanup();