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
			Camera* GetCurRenderingCamera();
		protected:
			void OnRender() override;
		private:
			void InitPasses();
			void OnFrameStart();
			void RenderCamera(Camera* camera);
			//
			DeferredShadingResources* m_resources;
			std::shared_ptr<DeferredShadingPass> m_parallelShadowPass;
			std::shared_ptr<DeferredShadingPass> m_gbufferPass;
			Camera* m_curRenderingCamera;
			XMFLOAT3 m_curRenderingCameraPosW;
		};
	}
}