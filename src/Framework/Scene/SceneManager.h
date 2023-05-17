#pragma once
#include "Entity.h"
#include "Camera.h"
#include "Light.h"
#include "../../math/MathLib.h"
#include "../../Color.h"
#include "../RenderPipeline/RenderPipeline.h"

namespace Framework 
{
	class FrameData;

	namespace SceneManager 
	{
		void Init(RenderPipeline* renderPipeline = nullptr);
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
		Camera* CreateDefaultMainCamera();

		Light* FindLight(const std::string name);
		Light* FindLight(const LightInstanceId id);
		Light* CreateLight(LIGHT_TYPE type,const std::string& name);
		bool DestroyLight(const LightInstanceId id);
		bool DestroyLight(Light* lit);

		void EnableLinearFog(bool enable);
		void SetLinearFogColor(RGBA32 color);
		void SetLinearFogStart(float start);
		void SetLinearFogRange(float range);

		void CollectFrameData(FrameData* frameData);
		void SetRenderPipeline(RenderPipeline* renderPipeline);
		void DrawOneFrame();
		void DrawOneFrame(Camera* camera);

		void Tick(float dt);
	}
}