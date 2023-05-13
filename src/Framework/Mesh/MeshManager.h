#pragma once

#include "../Resource.h"

namespace Framework 
{
	class Mesh;
	namespace MeshManager 
	{
		Mesh* FindWithUrl(Resource::ResourceUrl url);
		Mesh* LoadFromTxtFile(Resource::ResourceUrl url);
		Mesh* LoadFromObjFile(Resource::ResourceUrl url);
		void Cleanup();
		//
		Mesh* CreateQuad();
		Mesh* CreateFullScreenTriangle();
		Mesh* CreateMeshInstance();
		void DestroyMeshInstance(Mesh* meshInstance);
		void DestroyMeshInstance(Resource::ResourceGUID guid);
	}
}