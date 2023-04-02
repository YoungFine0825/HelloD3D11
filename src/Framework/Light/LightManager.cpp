
#include "LightManager.h"
#include "../Shader/Shader.h"

namespace Framework 
{
	namespace LightManager 
	{
		#define PARALLEL_LIGHT_COUNT 6
		ParallelLight m_parallelLights[PARALLEL_LIGHT_COUNT];
		#define POINT_LIGHT_COUNT 6
		PointLight m_pointLights[POINT_LIGHT_COUNT];
		#define SPOT_LIGHT_COUNT 6
		SpotLight m_spotLights[SPOT_LIGHT_COUNT];

		bool SetParallelLight(unsigned int index, ParallelLight light)
		{
			if (index >= PARALLEL_LIGHT_COUNT || index < 0) 
			{
				return false;
			}
			//
			m_parallelLights[index] = light;
			//
			return true;
		}

		ParallelLight* GetParallelLight(unsigned int index) 
		{
			if (index >= PARALLEL_LIGHT_COUNT || index < 0)
			{
				return nullptr;
			}
			return &m_parallelLights[index];
		}

		bool SetPointLight(unsigned int index, PointLight light) 
		{
			if (index >= POINT_LIGHT_COUNT || index < 0)
			{
				return false;
			}
			//
			m_pointLights[index] = light;
			//
			return true;
		}

		PointLight* GetPointLight(unsigned int index)
		{
			if (index >= POINT_LIGHT_COUNT || index < 0)
			{
				return nullptr;
			}
			return &m_pointLights[index];
		}

		bool SetSpotLight(unsigned int index, SpotLight light) 
		{
			if (index >= SPOT_LIGHT_COUNT || index < 0)
			{
				return false;
			}
			//
			m_spotLights[index] = light;
			//
			return true;
		}

		SpotLight* GetSpotLight(unsigned int index)
		{
			if (index >= SPOT_LIGHT_COUNT || index < 0)
			{
				return nullptr;
			}
			return &m_spotLights[index];
		}

		ParallelLight GetMaxParallelLight() 
		{
			ParallelLight ret = m_parallelLights[0];
			for (size_t i = 1; i < PARALLEL_LIGHT_COUNT; ++i) 
			{
				if (m_parallelLights[i].intensity > ret.intensity) 
				{
					ret = m_parallelLights[i];
				}
			}
			return ret;
		}
		//
		void PackLightParamtersToShader(Shader* shader)
		{
			ParallelLight parallelLight = GetMaxParallelLight();
			shader->SetStruct("g_ParallelLight", &parallelLight, sizeof(parallelLight));
			shader->SetStruct("g_PointLights", &m_pointLights, sizeof(m_pointLights));
			shader->SetStruct("g_SpotLights", &m_spotLights, sizeof(m_spotLights));
		}
	}
}