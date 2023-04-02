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

		void DoTick(float dt);
	protected:
		void virtual OnTick(float dt);
		Transform m_tranform;
		std::vector<Renderer*> m_renderers;
	private:
		EntityInstanceId m_id;
		std::string m_name;

	};
}