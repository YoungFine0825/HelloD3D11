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

		Mesh* mesh;
		Material* material;
		
		Entity* GetEntity();
		Renderer* SetEnable(bool enabled);
		bool IsEnabled();
		Renderer* EnableCastShadow(bool enabled);
		bool IsCastShadow();
		Renderer* EnableReceiveShadow(bool enabled);
		bool IsReceiveShadow();
	private:
		bool m_isEnabled = true;
		bool m_isCastShadow = true;
		bool m_isReceiveShadow = true;
		Entity* m_ent;
	};

	typedef std::vector<Renderer*> RendererVector;
}