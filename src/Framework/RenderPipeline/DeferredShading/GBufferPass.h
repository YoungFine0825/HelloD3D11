#pragma once
#include "../../math/MathLib.h"
#include "DeferredShadingPass.h"

namespace Framework
{
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
			void DrawGBuffer(UINT index, XMMATRIX viewMatrix,XMMATRIX projMatrix);
		};
	}
}