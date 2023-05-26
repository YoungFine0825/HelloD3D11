#pragma once

#include "../Material/Material.h"
#include "../Mesh/Mesh.h"
#include <vector>

namespace Framework 
{
	class Entity;
	typedef unsigned int RendererInstanceId;

	class Renderer 
	{
	public:
		Renderer();
		Renderer(Entity* ent);
		~Renderer();

		RendererInstanceId GetInstanceId();
		Entity* GetEntity();
		Renderer* SetEnable(bool enabled);
		bool IsEnabled();

		Material* GetMaterialInstance();
		Renderer* SetMaterialInstance(Material* inst);
		bool IsTransparent();

		Mesh* GetMeshInstance();
		Renderer* SetMeshInstance(Mesh* inst);
	private:
		RendererInstanceId m_id;
		bool m_isEnabled = true;
		Entity* m_ent;
		Material* m_material;
		Mesh* m_mesh;
	};

	typedef std::vector<Renderer*> RendererVector;
}