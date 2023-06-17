#pragma once
#include "DeferredShadingPass.h"

namespace Framework
{
	namespace DeferredShading
	{
		class TransparentPass : public DeferredShadingPass
		{
		public:
			TransparentPass();
			~TransparentPass() override;
			//
			void Init(DeferredRenderPipeline* rp, DeferredShadingResources* rps) override;
		protected:
			void OnFrameStart() override;
			void OnInvoke() override;
		private:

			struct LightParameter
			{
				XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
				XMFLOAT3 positionW = { 0,0,0 };
				float intensity = 0;
				XMFLOAT3 attenuation = { 1,1,1 };
				float range = 1;
				XMFLOAT3 directionW = { 0,0,1 };
				float spot = 1;

				LightParameter() : color({ 1.0f,1.0f,1.0f,1.0f }),
					positionW({ 0,0,0 }),
					intensity(0),
					attenuation({ 1,1,1 }),
					range(1),
					directionW({ 0,0,1 }),
					spot(1)
				{
				}
			};

			LightParameter m_parallelLightParam;
			LightParameter* m_pointLightsParam;
			LightParameter* m_spotLightsParam;
			unsigned int m_punctualLightCnt = 8;
		};
	}
}