#pragma once

#include "../Resource.h"
#include "Material.h"

namespace Framework 
{
	namespace MaterialManager 
	{
		Material* CreateMaterialInstance(Resource::ResourceUrl url);
		void Cleanup();
	}
}