#pragma once

#include "Light.h"

namespace Framework 
{
	class Shader;
	namespace LightManager 
	{
		bool SetParallelLight(unsigned int index, ParallelLight light);
		ParallelLight* GetParallelLight(unsigned int index);
		bool SetPointLight(unsigned int index, PointLight light);
		PointLight* GetPointLight(unsigned int index);
		bool SetSpotLight(unsigned int index, SpotLight light);
		SpotLight* GetSpotLight(unsigned int index);
		//
		void PackLightParamtersToShader(Shader* shader);
	}
}