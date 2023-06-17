#pragma once
#include "../../../math/MathLib.h"
#include "../RenderPipeline.h"
#include <memory>

namespace Framework 
{
	class Camera;
	class FrameData;
	class Renderer;
	class Shader;
	namespace DeferredShading 
	{
		class DeferredShadingResources;
		class DeferredShadingPass;
		class DeferredRenderPipeline : public RenderPipeline
		{
		public:
			DeferredRenderPipeline();
			~DeferredRenderPipeline() override;
			//
			FrameData* GetFrameData();
			void DrawRenderer(Renderer* renderer, Shader* shader,XMMATRIX viewMatrix,XMMATRIX projectMatrix,int pass = -1);
		protected:
			void OnRender() override;
		private:
			void InitPasses();
			void OnFrameStart();
			void RenderCamera(Camera* camera);
			//
			void FindParallelLight();
			void CollectVisiblePuntcualLights(Camera* camera);
			//
			DeferredShadingResources* m_resources;
			std::shared_ptr<DeferredShadingPass> m_shadowPass;
			std::shared_ptr<DeferredShadingPass> m_gbufferPass;
			std::shared_ptr<DeferredShadingPass> m_lightingPass;
			std::shared_ptr<DeferredShadingPass> m_backgroundPass;
			std::shared_ptr<DeferredShadingPass> m_unlightPass;
			std::shared_ptr<DeferredShadingPass> m_transparentPass;
		};
	}
}