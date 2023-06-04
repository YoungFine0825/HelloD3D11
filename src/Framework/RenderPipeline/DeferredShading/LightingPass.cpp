#include "../../../Window.h"
#include "../../Graphic.h"
#include "../../Shader/Shader.h"
#include "../../Shader/ShaderManager.h"
#include "../../Mesh/Mesh.h"
#include "../../Mesh/MeshManager.h"
#include "../../RenderTexture/RenderTexture.h"
#include "../../RenderTexture/RenderTextureManager.h"
#include "../../Light/Light.h"
#include "../FrameData.h"
#include "DeferredRenderPipeline.h"
#include "DeferredShadingResources.h"
#include "LightingPass.h"

namespace Framework 
{
	namespace DeferredShading 
	{
		LightingPass::LightingPass() : DeferredShadingPass()
		{
			m_lightingShader = ShaderManager::LoadFromFxFile("res/shaders/Pipeline/DeferredLighting.fx");
			m_sphereMesh = MeshManager::LoadFromObjFile("res/models/Pipeline/UnitSphere.obj");
			m_pyramidMesh = MeshManager::LoadFromTxtFile("res/models/Pipeline/UnitPyramid.txt");
			int winWidth = win_GetWidth();
			int winHeight = win_GetHeight();
			m_punctualLightsResult = RenderTextureManager::CreateRenderTexture(winWidth,winHeight,false);
		}

		LightingPass::~LightingPass() 
		{
			DeferredShadingPass::~DeferredShadingPass();
		}

		void LightingPass::Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps) 
		{
			DeferredShadingPass::Init(rp, rps);
		}

		void LightingPass::OnFrameStart() 
		{

		}

		void LightingPass::OnInvoke() 
		{
			Graphics::ClearRenderTexture(m_resouces->GetShadingResultTexture());
			ProcessParallelLight();
			ProcessPunctualLights();
		}

		void LightingPass::ProcessParallelLight() 
		{
			if (!m_lightingShader) 
			{
				return;
			}
			Light* parallelLit = m_resouces->GetParallelLight();
			if (!parallelLit)
			{
				return;
			}
			RenderingCameraInfo* cameraInfo = m_resouces->GetRenderingCameraInfo();
			//
			m_lightingShader
				->SetRenderTexture("g_GBufferTex0", m_resouces->GBuffer(0))
				->SetRenderTexture("g_GBufferTex1", m_resouces->GBuffer(1))
				->SetRenderTexture("g_GBufferTex2", m_resouces->GBuffer(2))
				->SetRenderTexture("g_GBufferTex3", m_resouces->GBuffer(3))
				->SetVector4("litColor", parallelLit->GetColor())
				->SetVector3("litDirectionW", parallelLit->GetTransform()->GetWorldSpaceForward())
				->SetFloat("litIntensity", parallelLit->GetIntensity())
				->SetVector3("g_CameraPosW", cameraInfo->posW)
				;
			//
			m_resouces->GetParallelShadowMap()->SetShaderParamters(m_lightingShader);
			//
			Graphics::Blit(m_resouces->GBuffer(0), m_resouces->GetShadingResultTexture(), m_lightingShader, 0);
		}

		void LightingPass::ProcessPunctualLights()
		{
			//
			Graphics::SetRenderTarget(m_resouces->GetShadingResultTexture());
			Graphics::SetDepthStencil(m_resouces->GetCameraDepthTexture());
			//Graphics::ClearBackground(Colors::White);
			//
			LightVector* lights = m_resouces->GetVisiblePunctualLights();
			size_t lightCnt = lights->size();
			for (size_t l = 0; l < lightCnt; ++l) 
			{
				Light* light = (*lights)[l];
				if (light->GetIntensity() <= 0) 
				{
					continue;
				}
				if (light->GetType() == LIGHT_TYPE_POINT) 
				{
					DrawPointLight(light);
				}
				else if (light->GetType() == LIGHT_TYPE_SPOT) 
				{
					DrawSpotLight(light);
				}
			}
		}

		void LightingPass::DrawPointLight(Light* light)
		{
			RenderingCameraInfo* cameraInfo = m_resouces->GetRenderingCameraInfo();
			XMFLOAT3 litPosW = light->GetTransform()->position;
			float litRange = light->GetRange();
			XMMATRIX translateW = XMMatrixTranslationFromFloat3(litPosW);
			XMMATRIX scaleW = XMMatrixScaling(litRange + 0.01f, litRange + 0.01f, litRange + 0.01f);
			XMMATRIX world = scaleW * translateW;
			m_lightingShader
				->SetRenderTexture("g_GBufferTex0", m_resouces->GBuffer(0))
				->SetRenderTexture("g_GBufferTex1", m_resouces->GBuffer(1))
				->SetRenderTexture("g_GBufferTex2", m_resouces->GBuffer(2))
				->SetRenderTexture("g_GBufferTex3", m_resouces->GBuffer(3))
				->SetVector4("litColor", light->GetColor())
				->SetVector3("litPositionW", litPosW)
				->SetVector3("litAttenuation", light->GetAttenuation())
				->SetFloat("litRange", litRange)
				->SetFloat("litIntensity", light->GetIntensity())
				->SetVector3("g_CameraPosW", cameraInfo->posW)
				->SetMatrix4x4("obj_MatMVP",world * cameraInfo->viewMatrix * cameraInfo->projMatrix)
				;
			bool isInside = XMFloat3Length(cameraInfo->posW - litPosW) <= litRange;
			//绘制点光源Volume
			UINT pass = isInside ? 1 : 2;
			Graphics::DrawMesh(m_sphereMesh, m_lightingShader, pass);
		}

		void LightingPass::DrawSpotLight(Light* light)
		{
			RenderingCameraInfo* cameraInfo = m_resouces->GetRenderingCameraInfo();
			Transform* litTrans = light->GetTransform();
			XMFLOAT3 litPosW = litTrans->position;
			float litRange = light->GetRange();
			float litSpot = light->GetSpot();
			float radin = Angle2Radin(litSpot);
			float radius = tan(radin) * litRange;
			XMFLOAT3 litDir = litTrans->GetWorldSpaceForward();
			XMMATRIX translateW = XMMatrixTranslationFromFloat3(litPosW);
			XMMATRIX scaleW = XMMatrixScaling(radius, radius, litRange);
			XMMATRIX world = scaleW * translateW;
			m_lightingShader
				->SetRenderTexture("g_GBufferTex0", m_resouces->GBuffer(0))
				->SetRenderTexture("g_GBufferTex1", m_resouces->GBuffer(1))
				->SetRenderTexture("g_GBufferTex2", m_resouces->GBuffer(2))
				->SetRenderTexture("g_GBufferTex3", m_resouces->GBuffer(3))
				->SetVector4("litColor", light->GetColor())
				->SetVector3("litPositionW", litPosW)
				->SetVector3("litDirectionW", litDir)
				->SetVector3("litAttenuation", light->GetAttenuation())
				->SetFloat("litRange", litRange)
				->SetFloat("litSpot", radin)
				->SetFloat("litIntensity", light->GetIntensity())
				->SetVector3("g_CameraPosW", cameraInfo->posW)
				->SetMatrix4x4("obj_MatMVP", world * cameraInfo->viewMatrix * cameraInfo->projMatrix)
				;
			XMFLOAT3 toLitPosW = cameraInfo->posW - litPosW;
			float toLitPosWLen = XMFloat3Length(toLitPosW);
			toLitPosW = XMVectorNormalize(toLitPosW);
			float dot = XMVectorDot(toLitPosW, XMVectorNormalize(litDir));
			bool isInside = dot > 0 && acos(dot) <= radin && toLitPosWLen <= litRange;
			//绘制点光源Volume
			UINT pass = isInside ? 3 : 4;
			Graphics::DrawMesh(m_pyramidMesh, m_lightingShader, pass);
		}
	}
}