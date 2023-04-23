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

	Renderer* Renderer::EnableCastShadow(bool enabled) 
	{
		m_isCastShadow = enabled;
		return this;
	}

	bool Renderer::IsCastShadow() 
	{
		return m_isCastShadow;
	}

	Renderer* Renderer::EnableReceiveShadow(bool enabled) 
	{
		m_isReceiveShadow = enabled;
		return this;
	}

	bool Renderer::IsReceiveShadow() 
	{
		return m_isReceiveShadow;
	}
}