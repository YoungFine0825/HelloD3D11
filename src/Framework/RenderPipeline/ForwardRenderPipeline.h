#pragma once
#include <vector>
#include "ShaderStruct.h"
#include "RenderPipeline.h"

namespace Framework 
{
	class ForwardRenderPipeline : public RenderPipeline 
	{
	public:
		ForwardRenderPipeline();
		~ForwardRenderPipeline();
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

		ShaderStruct::ParallelLight m_curParallelLight;

		RendererVector m_visibleRenderers;
		//
		void RenderCamera(Camera* camera);
		void GenLightList();
	};
}