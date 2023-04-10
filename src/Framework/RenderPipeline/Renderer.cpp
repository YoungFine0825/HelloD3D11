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
		mesh = nullptr;
		if (material != nullptr) 
		{
			delete material;
		}
	}

	void Renderer::SetEnable(bool enabled) 
	{
		m_isEnabled = enabled;
	}

	bool Renderer::IsEnabled() 
	{
		return m_isEnabled;
	}
	
	Entity* Renderer::GetEntity() 
	{
		return m_ent;
	}
}