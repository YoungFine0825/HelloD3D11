#pragma once
#include "Entity.h"
#include "Camera.h"
#include "../../math/MathLib.h"
#include "../../Color.h"

namespace Framework 
{
	namespace SceneManager 
	{
		void Init();
		void Cleanup();

		Entity* CreateEntity();
		Entity* CreateEntity(const std::string& name);
		bool DestroyEntity(Entity* ent);
		bool DestroyEntity(EntityInstanceId id);
		Entity* FindEntity(EntityInstanceId id);
		Entity* FindEntity(const std::string& name);
		bool AddEntity(Entity* ent);

		Camera* GetCamera(unsigned int index);
		Camera* GetCamera(const std::string& name);
		Camera* CreateCamera(const std::string& name);
		bool AddCamera(Camera* cam);
		std::vector<Camera*> GetCameras();
		bool DestroyCamera(Camera* cam);
		bool DestroyCamera(const std::string& name);

		void EnableLinearFog(bool enable);
		void SetLinearFogColor(RGBA32 color);
		void SetLinearFogStart(float start);
		void SetLinearFogRange(float range);

		void DrawOneFrame();
		void DrawOneFrame(Camera* camera);
		void DrawOneFrame(XNA::Frustum frustm, XMMATRIX viewMatrix, XMMATRIX projectMatrix);

		void Tick(float dt);
	}
}