#pragma once

#include "../Resource.h"

namespace Framework 
{
	class Shader;
	namespace ShaderManager 
	{
		Shader* FindWithUrl(Resource::ResourceUrl url);
		Shader* LoadFromFxFile(Resource::ResourceUrl url);
		void Cleanup();
	}
}