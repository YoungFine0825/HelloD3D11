#pragma once

#include "../Resource.h"
#include "../Mesh/Mesh.h"

#include <vector>

namespace Framework 
{
	class Model : public Resource::IResource
	{
	public:
		Model();
		~Model() override;
		//
		void Release() override;
		//
		Mesh* CreateMesh();
		Mesh* GetMesh(UINT meshIndex);
		UINT GetMeshCount() { return m_meshes.size(); }
		//
		Model* AddMesh(Mesh* mesh);
		Model* UploadMeshes();
	private:
		std::vector<Mesh*> m_meshes;
	};
}