#include "RenderSystem.h"
#include "FrameData.h"
#include "../../Global.h"
#include "../Graphic.h"
#include "../ImGUI/ImGuiHelper.h"
#include "../Utils/GizmosHelper.h"
#include "../Scene/SceneManager.h"
#include "DeferredShading/DeferredRenderPipeline.h"

namespace Framework 
{
	namespace RenderSystem 
	{
		DeferredShading::DeferredRenderPipeline* m_defaultRenderPipeline;
		RenderPipeline* m_activedRenderPipline;
		FrameData* m_frameData;
		RenderPipelineDrawGUICallback m_drawGUICallBack;
		RenderPipelineDrawGizmosCallback m_drawGizmosCallBack;

		RenderSystemErrorCode Init()
		{
			if (!d3dGraphic::Init()) 
			{
				return RENDER_SYSTEM_ERROR_INIT_D3D_FAILED;
			}
			GizmosHelper::Init();
			ImGuiHelper::Init();
			m_frameData = new FrameData();
			m_defaultRenderPipeline = new DeferredShading::DeferredRenderPipeline();
			m_activedRenderPipline = m_defaultRenderPipeline;
			return RENDER_SYSTEM_ERROR_NONE;
		}

		void Cleanup()
		{
			m_activedRenderPipline = nullptr;
			ReleasePointer(m_defaultRenderPipeline);
			ReleasePointer(m_frameData);
			GizmosHelper::Cleanup();
			ImGuiHelper::Cleanup();
			d3dGraphic::Shutdown();
		}

		void SetDrawGUICallBack(RenderPipelineDrawGUICallback cb) 
		{
			m_drawGUICallBack = cb;
			m_activedRenderPipline->SetDrawGUICallBack(m_drawGUICallBack);
		}

		void SetDrawGizmosCallBack(RenderPipelineDrawGizmosCallback cb) 
		{
			m_drawGizmosCallBack = cb;
			m_activedRenderPipline->SetDrawGizmosCallBack(m_drawGizmosCallBack);
		}

		void SetRenderPipeline(RenderPipeline* pipline) 
		{
			if (pipline) 
			{
				m_activedRenderPipline = pipline;
				m_activedRenderPipline->SetDrawGUICallBack(m_drawGUICallBack);
				m_activedRenderPipline->SetDrawGizmosCallBack(m_drawGizmosCallBack);
			}
			else 
			{
				m_activedRenderPipline = m_defaultRenderPipeline;
			}
		}

		RenderSystemErrorCode RenderFrame()
		{
			if (!m_activedRenderPipline) 
			{
				return RENDER_SYSTEM_ERROR_INVALID_PIPLINE;
			}
			m_frameData->Reset();
			SceneManager::CollectFrameData(m_frameData);
			m_activedRenderPipline->DoRender(m_frameData);
			return RENDER_SYSTEM_ERROR_NONE;
		}

		RenderSystemErrorCode RenderCamera(Camera* camera) 
		{
			if (!m_activedRenderPipline)
			{
				return RENDER_SYSTEM_ERROR_INVALID_PIPLINE;
			}
			if (!camera || !camera->IsEnabled())
			{
				return RENDER_SYSTEM_ERROR_INVALID_ARGS;
			}
			m_frameData->Reset();
			SceneManager::CollectFrameData(m_frameData);
			m_frameData->cameras.clear();
			m_frameData->cameras.push_back(camera);
			m_activedRenderPipline->DoRender(m_frameData);
			return RENDER_SYSTEM_ERROR_NONE;
		}
	}
}