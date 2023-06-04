#pragma once
#include "DeferredShadingPass.h"

namespace Framework
{
	class Shader;
	class Mesh;
	class Light;
	class RenderTexture;
	namespace DeferredShading
	{
		class LightingPass : public DeferredShadingPass
		{
		public:
			LightingPass();
			~LightingPass() override;
			//
			void Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps) override;
		protected:
			void OnFrameStart() override;
			void OnInvoke() override;
		private:
			void ProcessParallelLight();
			void ProcessPunctualLights();
			void DrawPointLight(Light* light);
			void DrawSpotLight(Light* light);
			Shader* m_lightingShader;
			Mesh* m_sphereMesh;
			Mesh* m_pyramidMesh;
			RenderTexture* m_punctualLightsResult;
		};
	}
}