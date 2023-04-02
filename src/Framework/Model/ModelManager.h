#pragma once
#include "../Resource.h"

namespace Framework 
{
	class Model;
	namespace ModelManager 
	{
		Model* FindWithUrl(Resource::ResourceUrl url);
		Model* LoadFromObjFile(Resource::ResourceUrl url);
		void Cleanup();
	}
}