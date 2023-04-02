#pragma once

#include "Transform.h"
#include "Renderer.h"
#include <vector>

namespace Framework 
{
	typedef unsigned int EntityInstanceId;

	class Entity 
	{
	public:
		Entity();
		Entity(std::string name);
		~Entity();

		EntityInstanceId GetInstanceId();
		std::string GetName();
		Transform* GetTransform();

		Renderer* CreateRenderer();
		Renderer* GetRenderer(unsigned int index);
		unsigned int GetRendererCount();

	private:
		EntityInstanceId m_id;
		std::string m_name;
		Transform m_tranform;
		std::vector<Renderer*> m_renderers;
	};
}