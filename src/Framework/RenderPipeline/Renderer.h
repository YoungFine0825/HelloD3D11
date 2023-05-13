#pragma once

#include "../Material/Material.h"
#include "../Mesh/Mesh.h"
#include <vector>

namespace Framework 
{
	class Entity;
	class Renderer 
	{
	public:
		Renderer(Entity* ent);
		~Renderer();

		Entity* GetEntity();
		Renderer* SetEnable(bool enabled);
		bool IsEnabled();

		Material* GetMaterialInstance();
		Renderer* SetMaterialInstance(Material* inst);

		Mesh* GetMeshInstance();
		Renderer* SetMeshInstance(Mesh* inst);
	private:
		bool m_isEnabled = true;
		Entity* m_ent;
		Material* m_material;
		Mesh* m_mesh;
	};

	typedef std::vector<Renderer*> RendererVector;
}