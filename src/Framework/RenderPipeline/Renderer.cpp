#include "Renderer.h"

namespace Framework 
{
	Renderer::Renderer(Entity* ent) 
	{
		m_ent = ent;
	}

	Renderer::~Renderer() 
	{
		m_ent = nullptr;
		m_mesh = nullptr;
		if (m_material != nullptr) 
		{
			delete m_material;
		}
	}

	Renderer* Renderer::SetEnable(bool enabled) 
	{
		m_isEnabled = enabled;
		return this;
	}

	bool Renderer::IsEnabled() 
	{
		return m_isEnabled;
	}
	
	Entity* Renderer::GetEntity() 
	{
		return m_ent;
	}

	Material* Renderer::GetMaterialInstance() 
	{
		return m_material;
	}

	Renderer* Renderer::SetMaterialInstance(Material* inst) 
	{
		if (m_material) 
		{
			delete m_material;
		}
		m_material = inst;
		return this;
	}

	Mesh* Renderer::GetMeshInstance() 
	{
		return m_mesh;
	}

	Renderer* Renderer::SetMeshInstance(Mesh* inst)
	{
		m_mesh = inst;
		return this;
	}
}