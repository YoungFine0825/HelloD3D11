#pragma once
#include "../../math/MathLib.h"
#include "DeferredShadingPass.h"

namespace Framework
{
	class Shader;
	namespace DeferredShading
	{
		class GBufferPass : public DeferredShadingPass
		{
		public:
			GBufferPass();
			~GBufferPass() override;
			//
			void Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps) override;
		protected:
			void OnFrameStart() override;
			void OnInvoke() override;
		private:
			void DrawGBuffer(UINT index, Shader* shader,XMMATRIX viewMatrix,XMMATRIX projMatrix);
		};
	}
}