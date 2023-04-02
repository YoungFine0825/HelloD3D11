
#include "Model.h"

namespace Framework 
{
	Model::Model() :IResource() 
	{

	}

	Model::~Model() 
	{
		IResource::~IResource();
	}

	void Model::Release() 
	{
		for (size_t i = 0; i < m_meshes.size(); ++i) 
		{
			Resource::ReleaseWithGUID(m_meshes[i]->GetGUID());
		}
		m_meshes.clear();
		IResource::Release();
	}

	Mesh* Model::CreateMesh() 
	{
		Mesh* mesh = new Mesh();
		m_meshes.push_back(mesh);
		return mesh;
	}

	Mesh* Model::GetMesh(UINT meshIndex) 
	{
		if (meshIndex >= m_meshes.size()) 
		{
			return nullptr;
		}
		Mesh* mesh = m_meshes[meshIndex];
		return mesh;
	}

	Model* Model::AddMesh(Mesh* mesh) 
	{
		m_meshes.push_back(mesh);
		return this;
	}

	Model* Model::UploadMeshes() 
	{
		for (size_t i = 0; i < m_meshes.size(); ++i)
		{
			m_meshes[i]->UpLoad();
		}
		return this;
	}
}