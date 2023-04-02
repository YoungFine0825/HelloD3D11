#pragma once

#include "../Material/Material.h"
#include "../Mesh/Mesh.h"

namespace Framework 
{
	class Entity;
	class Renderer 
	{
	public:
		Renderer(Entity* ent);
		~Renderer();

		Mesh* mesh;
		Material* material;

		Entity* GetEntity();
		void SetEnable(bool enabled);
		bool IsEnabled();
	private:
		bool m_isEnabled = true;
		Entity* m_ent;
	};
}