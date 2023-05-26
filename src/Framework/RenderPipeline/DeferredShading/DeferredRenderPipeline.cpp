#include "../../../App.h"
#include "../../Scene/Camera.h"
#include "../../Scene/Entity.h"
#include "../../Material/Material.h"
#include "../../Shader/Shader.h"
#include "../../Graphic.h"
#include "../FrameData.h"
#include "../Renderer.h"
#include "DeferredRenderPipeline.h"
#include "DeferredShadingResources.h"
//Passes
#include "DeferredShadingPass.h"
#include "ParallelShadowPass.h"
#include "GBufferPass.h"

namespace Framework
{
	namespace DeferredShading
	{
		DeferredRenderPipeline::DeferredRenderPipeline()
		{
			m_resources = new DeferredShadingResources();
			InitPasses();
		}

		DeferredRenderPipeline::~DeferredRenderPipeline()
		{
			if (m_resources)
			{
				delete m_resources;
				m_resources = nullptr;
			}
			RenderPipeline::~RenderPipeline();
		}

		void DeferredRenderPipeline::InitPasses()
		{
			m_parallelShadowPass = std::make_shared<DeferredShading::ParallelShadowPass>();
			m_gbufferPass = std::make_shared<DeferredShading::GBufferPass>();
			//
			m_parallelShadowPass->Init(this, this->m_resources);
			m_gbufferPass->Init(this, this->m_resources);
		}

		FrameData* DeferredRenderPipeline::GetFrameData() 
		{
			return m_frameData;
		}

		void DeferredRenderPipeline::OnRender()
		{
			size_t cameraCnt = m_frameData->cameras.size();
			//
			OnFrameStart();
			//Draw Camera
			for (size_t c = 0; c < cameraCnt; ++c)
			{
				RenderCamera(m_frameData->cameras[c]);
				//
				if (m_drawGizmosCallBack)
				{
					m_drawGizmosCallBack(m_frameData->cameras[c]);
				}
				//
				if (m_drawGUICallBack)
				{
					m_drawGUICallBack(m_frameData->cameras[c]);
				}
			}
			//输出到屏幕
			Graphics::Present();
		}

		void DeferredRenderPipeline::OnFrameStart() 
		{
			m_parallelShadowPass->OnFrameStart();
			m_gbufferPass->OnFrameStart();
		}

		void DeferredRenderPipeline::RenderCamera(Camera* camera)
		{
			RendererVector visibleRenderers;
			CollectVisibleRenderers(camera, &visibleRenderers);
			//
			size_t rendererCnt = visibleRenderers.size();
			if (rendererCnt <= 0)
			{
				return;
			}
			//
			SortVisibleRenderers(camera, &visibleRenderers);
			//
			m_resources->m_visibleOpaqueRenderers.clear();
			m_resources->m_visibleTranparentRenderers.clear();
			for (size_t r = 0; r < rendererCnt; ++r) 
			{
				if (visibleRenderers[r]->IsTransparent()) 
				{
					m_resources->m_visibleTranparentRenderers.push_back(visibleRenderers[r]);
				}
				else 
				{
					m_resources->m_visibleOpaqueRenderers.push_back(visibleRenderers[r]);
				}
			}
			//
			m_curRenderingCamera = camera;
			m_curRenderingCameraPosW = camera->GetTransform()->position;
			//
			//static_cast<ParallelShadowPass*>(m_parallelShadowPass.get())->Invoke();
			//
			static_cast<GBufferPass*>(m_gbufferPass.get())->Invoke();
		}

		void DeferredRenderPipeline::DrawRenderer(Renderer* renderer, Shader* shader, XMMATRIX viewMatrix, XMMATRIX projectMatrix,int pass)
		{
			Entity* ent = renderer->GetEntity();
			Material* mat = renderer->GetMaterialInstance();
			//Per Frame
			shader->SetFloat("g_timeDelta", App_GetTimeDelta());
			shader->SetVector3("g_CameraPosW", m_curRenderingCameraPosW);
			//设置Tranform参数
			Transform* trans = ent->GetTransform();
			XMMATRIX worldMat = trans->GetWorldMatrix();
			shader->SetMatrix4x4("obj_MatWorld", worldMat);
			XMMATRIX normalWorldMat = XMMatrixInverseTranspose(worldMat);
			shader->SetMatrix4x4("obj_MatNormalWorld", normalWorldMat);
			XMMATRIX mvp = worldMat * viewMatrix * projectMatrix;
			shader->SetMatrix4x4("obj_MatMVP", mvp);
			shader->SetMatrix4x4("obj_MatView", viewMatrix);
			shader->SetMatrix4x4("obj_MatProj", projectMatrix);
			//
			mat->Apply();
			//绘制
			Graphics::DrawMesh(renderer->GetMeshInstance(), shader,pass);
		}

		Camera* DeferredRenderPipeline::GetCurRenderingCamera() 
		{
			return m_curRenderingCamera;
		}
	}

}