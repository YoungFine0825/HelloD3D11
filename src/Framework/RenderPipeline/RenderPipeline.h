#pragma once
#include <functional>
#include "Renderer.h"

namespace Framework 
{
	class FrameData;
	class Camera;
	class Renderer;

	typedef std::function<void()> RenderPipelineDrawGUICallback;
	typedef std::function<void()> RenderPipelineDrawGizmosCallback;

	class RenderPipeline 
	{
	public:
		RenderPipeline();
		~RenderPipeline();
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