#include <vector>
#include <algorithm>
#include "SceneManager.h"
#include "../../Global.h"
#include "../../Window.h"
#include "../../App.h"
#include "../../math/MathLib.h"
#include "../RenderPipeline/FrameData.h"
#include "../RenderPipeline/RenderSystem.h"

namespace Framework 
{
	namespace SceneManager 
	{
		typedef std::vector<Framework::Entity*> EntityList;
		typedef std::vector<Framework::Camera*> CameraList;
		EntityList m_entities;
		CameraList m_cameras;

		typedef std::vector<Framework::Renderer*> RendererList;
		RendererList m_renderableRenderers;

		typedef std::vector<Framework::Light*> LightList;
		LightList m_lights;

		bool m_enableFog = true;
		RGBA32 m_linearFogColor = { 0.5f,0.5f,0.5f,1.0f };
		float m_linearFogStart = 100;
		float m_linearFogRange = 2000;


		void Init(RenderPipeline* renderPipeline)
		{
			SetRenderPipeline(renderPipeline);
		}

		void Cleanup() 
		{
			//
			for (size_t i = 0; i < m_entities.size(); ++i) 
			{
				delete m_entities[i];
			}
			m_entities.clear();
			//
			for (size_t i = 0; i < m_cameras.size(); ++i)
			{
				delete m_cameras[i];
			}
			m_cameras.clear();
			//
			for (size_t i = 0; i < m_lights.size(); ++i)
			{
				delete m_lights[i];
			}
			m_lights.clear();
		}

		Entity* CreateEntity() 
		{
			std::string name = "Unknow" + (m_entities.size() + 1);
			return CreateEntity(name);
		}

		Entity* CreateEntity(const std::string& name) 
		{
			Entity* ent = new Entity(name);
			m_entities.push_back(ent);
			return ent;
		}

		bool DestroyEntity(EntityInstanceId id) 
		{
			EntityList::iterator it = m_entities.begin();
			EntityList::iterator end = m_entities.end();
			Entity* ent = nullptr;
			for (; it != end; ++it)
			{
				if ((*it)->GetInstanceId() == id) 
				{
					ent = *it;
					break;
				}
			}
			//
			if (ent == nullptr) 
			{
				return false;
			}
			//
			m_entities.erase(it);
			delete ent;
			return true;
		}

		bool DestroyEntity(Entity* ent)
		{
			return DestroyEntity(ent->GetInstanceId());
		}

		Entity* FindEntity(EntityInstanceId id) 
		{
			size_t entCnt = m_entities.size();
			for (size_t i = 0; i < entCnt; ++i) 
			{
				if (m_entities[i]->GetInstanceId() == id) 
				{
					return m_entities[i];
				}
			}
			return nullptr;
		}

		Entity* FindEntity(const std::string& name) 
		{
			size_t entCnt = m_entities.size();
			for (size_t i = 0; i < entCnt; ++i)
			{
				if (m_entities[i]->GetName() == name)
				{
					return m_entities[i];
				}
			}
			return nullptr;
		}

		bool AddEntity(Entity* ent) 
		{
			Entity* existed = FindEntity(ent->GetInstanceId());
			if (existed != nullptr) 
			{
				return false;
			}
			m_entities.push_back(ent);
			return true;
		}


		Camera* GetCamera(unsigned int index) 
		{
			if (index >= m_cameras.size()) 
			{
				return nullptr;
			}
			Camera* cam = m_cameras[index];
			return cam;
		}

		Camera* GetCamera(const std::string& name) 
		{
			Camera* cam = nullptr;
			for (size_t i = 0; i < m_cameras.size(); ++i) 
			{
				if (m_cameras[i]->GetName() == name) 
				{
					cam = m_cameras[i];
					break;
				}
			}
			return cam;
		}

		Camera* CreateCamera(const std::string& name) 
		{
			Camera* created = GetCamera(name);
			if (created != nullptr) 
			{
				return created;
			}
			//
			Camera* cam = new Camera(name);
			m_cameras.push_back(cam);
			return cam;
		}

		bool AddCamera(Camera* cam)
		{
			Camera* created = GetCamera(cam->GetName());
			if (created != nullptr) 
			{
				return false;
			}
			m_cameras.push_back(cam);
			return true;
		}

		std::vector<Camera*> GetCameras() 
		{
			return m_cameras;
		}

		bool DestroyCamera(Camera* cam) 
		{
			return DestroyCamera(cam->GetName());
		}

		bool DestroyCamera(const std::string& name)
		{
			CameraList::iterator it = m_cameras.begin();
			CameraList::iterator end = m_cameras.end();
			Camera* cam = nullptr;
			for (; it != end; ++it) 
			{
				if ((*it)->GetName() == name) 
				{
					cam = *it;
					break;
				}
			}
			if (cam == nullptr) 
			{
				return false;
			}
			m_cameras.erase(it);
			delete cam;
			return true;
		}

		Camera* CreateDefaultMainCamera() 
		{
			Camera* mainCamera = GetCamera("MainCamera");
			if (mainCamera)
			{
				return mainCamera;
			}
			mainCamera = SceneManager::CreateCamera("MainCamera");
			mainCamera->SetAspectRatio(win_GetAspectRatio())
				->SetClearFlag(CAMERA_CLEAR_SOLID_COLOR)
				->SetNearClipDistance(1.0f)
				->SetFarClipDistance(5000)
				->SetFov(45)
				;
			return mainCamera;
		}


		Light* FindLight(const std::string name) 
		{
			Light* lit = nullptr;
			for (size_t i = 0; i < m_lights.size(); ++i)
			{
				if (m_lights[i]->GetName() == name)
				{
					lit = m_lights[i];
					break;
				}
			}
			return lit;
		}

		Light* FindLight(const LightInstanceId id) 
		{
			Light* lit = nullptr;
			for (size_t i = 0; i < m_lights.size(); ++i)
			{
				if (m_lights[i]->GetId() == id)
				{
					lit = m_lights[i];
					break;
				}
			}
			return lit;
		}

		Light* CreateLight(LIGHT_TYPE type, const std::string& name) 
		{
			Light* lit = new Light(type);
			lit->SetName(name);
			m_lights.push_back(lit);
			return lit;
		}

		bool DestroyLight(const LightInstanceId id) 
		{
			LightList::iterator it = m_lights.begin();
			LightList::iterator end = m_lights.end();
			Light* lit = nullptr;
			for (; it != end; ++it)
			{
				if ((*it)->GetId() == id)
				{
					lit = *it;
					break;
				}
			}
			if (lit == nullptr)
			{
				return false;
			}
			m_lights.erase(it);
			delete lit;
			return true;
		}

		bool DestroyLight(Light* lit) 
		{
			if (!lit) 
			{
				return false;
			}
			return DestroyLight(lit->GetId());
		}


		void EnableLinearFog(bool enable) 
		{
			m_enableFog = enable;
		}

		void SetLinearFogColor(RGBA32 color)
		{
			m_linearFogColor = color;
		}

		void SetLinearFogStart(float start)
		{
			m_linearFogStart = start;
		}

		void SetLinearFogRange(float range)
		{
			m_linearFogRange = range;
		}

		void SetRenderPipeline(RenderPipeline* renderPipeline) 
		{
			RenderSystem::SetRenderPipeline(renderPipeline);
		}


		bool isRenderValid(Renderer* r) 
		{
			bool ret = (r->IsEnabled() && r->GetMeshInstance() != nullptr && r->GetMaterialInstance() != nullptr && r->GetMaterialInstance()->GetShader() != nullptr);
			return ret;
		}

		void CollectFrameData(FrameData* frameData) 
		{
			frameData->sceneSetting.enableFog = m_enableFog;
			frameData->sceneSetting.linearFogColor = m_linearFogColor;
			frameData->sceneSetting.linearFogStart = m_linearFogStart;
			frameData->sceneSetting.linearFogRange = m_linearFogRange;
			//收集摄像机
			for (size_t i = 0; i < m_cameras.size(); ++i)
			{
				if (!m_cameras[i]->IsEnabled())
				{
					continue;
				}
				//
				frameData->cameras.push_back(m_cameras[i]);
			}
			//收集光源
			for (size_t i = 0; i < m_lights.size(); ++i)
			{
				if (m_lights[i]->IsEnabled())
				{
					frameData->lights.push_back(m_lights[i]);
				}
			}
			//收集renderer
			for (size_t i = 0; i < m_entities.size(); ++i)
			{
				if (!m_entities[i]->IsEnabled())
				{
					continue;
				}
				//
				unsigned int rendererCnt = m_entities[i]->GetRendererCount();
				if (rendererCnt > 0)
				{
					for (unsigned int rIdx = 0; rIdx < rendererCnt; ++rIdx)
					{
						Renderer* r = m_entities[i]->GetRenderer(rIdx);
						if (isRenderValid(r))
						{
							frameData->renderers.push_back(r);
						}
					}
				}
			}
		}

		void DrawOneFrame() 
		{
			RenderSystem::RenderFrame();
		}


		void DrawOneFrame(Camera* camera) 
		{
			RenderSystem::RenderCamera(camera);
		}

		void Tick(float dt) 
		{
			for (size_t i = 0; i < m_entities.size(); ++i)
			{
				if (m_entities[i]->IsEnabled()) 
				{
					m_entities[i]->DoTick(dt);
				}
			}
		}
	}
}