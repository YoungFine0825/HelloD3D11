#pragma once
#include <vector>
#include "ShaderStruct.h"
#include "RenderPipeline.h"

namespace Framework 
{
	class ParallelLightShadowMap;

	class ForwardRenderPipeline : public RenderPipeline 
	{
	public:
		ForwardRenderPipeline();
		~ForwardRenderPipeline();
		//
		ForwardRenderPipeline* SetShadowMapSize(unsigned int size);
	protected:
		void OnRender() override;
	private:
		int m_pointLightCnt = 6;
		int m_spotLightCnt = 6;

		typedef std::vector<ShaderStruct::PointLight> PointLightArray;

		typedef std::vector<ShaderStruct::SpotLight> SpotLightArray;

		struct InteractedLightSet
		{
			InteractedLightSet() {};
			PointLightArray pointLights;
			SpotLightArray spotLights;
		};

		std::vector<InteractedLightSet*> m_lightLists;

		ShaderStruct::ParallelLight m_maxIntensityParallelLight;
		XMFLOAT3 m_maxIntensityParallelLightRotW;

		RendererVector m_visibleRenderers;
		//
		void RenderCamera(Camera* camera);
		void FindMaxIntensityParallelLight();
		void GenLightList();
		//
		ParallelLightShadowMap* m_shadowMap;
		void DrawShadowMap();
	};
}