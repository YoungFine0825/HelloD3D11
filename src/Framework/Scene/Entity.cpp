
#include "Entity.h"

namespace Framework 
{
	EntityInstanceId g_entityId = 0;

	Entity::Entity() 
	{
		m_id = ++g_entityId;
	}

	Entity::Entity(std::string name) 
	{
		m_name = name;
		m_id = ++g_entityId;
	}

	Entity::~Entity() 
	{
		for (size_t i = 0; i < m_renderers.size(); i++) 
		{
			delete m_renderers[i];
		}
		m_renderers.clear();
	}

	EntityInstanceId Entity::GetInstanceId()
	{
		return m_id;
	}

	std::string Entity::GetName() 
	{
		return m_name;
	}

	Transform* Entity::GetTransform() 
	{
		return &m_tranform;
	}

	Renderer* Entity::CreateRenderer()
	{
		Renderer* r = new Renderer(this);
		m_renderers.push_back(r);
		return r;
	}

	Renderer* Entity::GetRenderer(unsigned int index) 
	{
		if (index >= m_renderers.size()) 
		{
			return nullptr;
		}
		return m_renderers[index];
	}

	unsigned int Entity::GetRendererCount() 
	{
		return m_renderers.size();
	}
}