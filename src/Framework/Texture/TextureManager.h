#pragma once

#include "../Resource.h"

namespace Framework 
{
	class Texture;
	namespace TextureManager 
	{
		Texture* FindWithUrl(Resource::ResourceUrl url);
		void Cleanup();
		//
		Texture* LoadDDSFromFile(Resource::ResourceUrl url);
		Texture* LoadFromFile(Resource::ResourceUrl url);
	}
}