#pragma once
#include <functional>
#include "Renderer.h"

namespace Framework 
{
	class FrameData;
	class Camera;

	typedef std::function<void(Camera*)> RenderPipelineDrawGUICallback;
	typedef std::function<void(Camera*)> RenderPipelineDrawGizmosCallback;

	class RenderPipeline 
	{
	public:
		RenderPipeline();
		virtual ~RenderPipeline();
		//
		void DoRender(FrameData* frameData);
		void SetDrawGUICallBack(RenderPipelineDrawGUICallback cb);
		void SetDrawGizmosCallBack(RenderPipelineDrawGizmosCallback cb);
	protected:
		virtual void OnRender();
		//
		void SetupCamera(Camera* camera);
		void CollectVisibleRenderers(Camera* camera, RendererVector* visibleRenderers);
		void SortVisibleRenderers(Camera* camera, RendererVector* visibleRenderers);
		//
		FrameData* m_frameData;
		RenderPipelineDrawGUICallback m_drawGUICallBack;
		RenderPipelineDrawGizmosCallback m_drawGizmosCallBack;
	};
}