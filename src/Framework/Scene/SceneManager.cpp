#include <vector>
#include <algorithm>
#include "SceneManager.h"
#include "../../Global.h"
#include "../Graphic.h"
#include "../Light/LightManager.h"
#include "../RenderTexture/RenderTexture.h"

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

		bool m_enableFog = true;
		RGBA32 m_linearFogColor = { 0.5f,0.5f,0.5f,1.0f };
		float m_linearFogStart = 100;
		float m_linearFogRange = 2000;

		void Init() 
		{

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


		void DrawOneFrame() 
		{
			if (m_cameras.size() <= 0) 
			{
				return;
			}
			//
			for (size_t i = 0; i < m_cameras.size(); ++i) 
			{
				if (!m_cameras[i]->IsEnabled()) 
				{
					continue;
				}
				//
				DrawOneFrame(m_cameras[i]);
			}
		}


		void DrawOneFrame(Camera* camera) 
		{
			XMMATRIX view = camera->GetViewMatrix();
			XMMATRIX proj = camera->GetProjectMatrix();
			//
			Framework::RenderTexture* rt = camera->GetRenderTexture();
			Graphics::SetRenderTarget(rt);
			//
			CameraClearFlag clearFlag = camera->GetClearFlag();
			switch (clearFlag)
			{
			case Framework::CAMERA_CLEAR_SOLID_COLOR:
				Graphics::ClearBackground(camera->GetBackgroundColor());
				Graphics::ClearDepthStencil();
				break;
			case Framework::CAMERA_CLEAR_SKYBOX:
				Graphics::ClearBackground(camera->GetBackgroundColor());
				Graphics::ClearDepthStencil();
				break;
			case Framework::CAMERA_CLEAR_ONLY_DEPTHl:
				Graphics::ClearDepthStencil(D3D11_CLEAR_DEPTH);
				break;
			default:
				break;
			}
			//
			DrawOneFrame(view, proj);
		}

		void CollectRenderableRenderers(RendererList* list) 
		{
			for (size_t i = 0; i < m_entities.size(); ++i) 
			{
				unsigned int rendererCnt = m_entities[i]->GetRendererCount();
				if (rendererCnt > 0)
				{
					for (unsigned int rIdx = 0; rIdx < rendererCnt; ++rIdx)
					{
						Renderer* r = m_entities[i]->GetRenderer(rIdx);
						if (r->IsEnabled() && r->mesh != nullptr && r->material != nullptr && r->material->GetShader() != nullptr) 
						{
							list->push_back(r);
						}
					}
				}
			}
		}

		void SortingRenderers(RendererList* list, XMMATRIX viewMatrix)
		{
			sort(list->begin(), list->end(), 
				[viewMatrix](Renderer* a, Renderer* b)
				{
					if (a->material->renderQueue == b->material->renderQueue)
					{
						if (a->material->renderQueue == RENDER_QUEUE_TRANSPARENT)
						{
							XMFLOAT3 entPos = a->GetEntity()->GetTransform()->position;
							XMFLOAT3 posA = XMFloat3MultiMatrix(entPos, viewMatrix);
							//
							entPos = b->GetEntity()->GetTransform()->position;
							XMFLOAT3 posB = XMFloat3MultiMatrix(entPos, viewMatrix);
							//
							return posA.z > posB.z;
						}
						else
						{
							return a->GetEntity()->GetInstanceId() < b->GetEntity()->GetInstanceId();
						}
					}
					return a->material->renderQueue <= b->material->renderQueue;
				}
			);
		}

		void DrawRenderer(Renderer* r, XMFLOAT3 cameraPosW, XMMATRIX viewMatrix, XMMATRIX projectMatrix)
		{
			Entity* ent = r->GetEntity();
			Material* mat = r->material;
			Shader* sh = mat->GetShader();
			//
			if (m_enableFog)
			{
				sh->SetEnabledTechnique("UseLinearFog");
				sh->SetVector4("g_linearFogColor", m_linearFogColor);
				sh->SetFloat("g_linearFogStart", m_linearFogStart);
				sh->SetFloat("g_linearFogRange", m_linearFogRange);
			}
			else
			{
				sh->SetEnabledTechnique("Default");
			}
			//
			sh->SetVector3("g_CameraPosW", cameraPosW);
			//
			LightManager::PackLightParamtersToShader(sh);
			//
			Transform* trans = ent->GetTransform();
			XMMATRIX worldMat = trans->GetWorldMatrix();
			sh->SetMatrix4x4("obj_MatWorld", worldMat);
			XMMATRIX normalWorldMat = XMMatrixInverseTranspose(worldMat);
			sh->SetMatrix4x4("obj_MatNormalWorld", normalWorldMat);
			XMMATRIX mvp = worldMat * viewMatrix * projectMatrix;
			sh->SetMatrix4x4("obj_MatMVP", mvp);
			//
			mat->Apply();
			//
			Graphics::DrawMesh(r->mesh, sh);
		}

		void DrawOneFrame(XMMATRIX viewMatrix, XMMATRIX projectMatrix) 
		{
			m_renderableRenderers.clear();
			//
			CollectRenderableRenderers(&m_renderableRenderers);
			//
			if (m_renderableRenderers.size() <= 0) 
			{
				return;
			}
			//
			SortingRenderers(&m_renderableRenderers, viewMatrix);
			//
			XMFLOAT3 cameraPosW = XMFloat3MultiMatrix({ 0,0,0 }, XMMatrixInverse(viewMatrix));
			for (size_t i = 0; i < m_renderableRenderers.size(); ++i) 
			{
				DrawRenderer(m_renderableRenderers[i],cameraPosW,viewMatrix,projectMatrix);
			}
		}

		void Tick(float dt) 
		{
			for (size_t i = 0; i < m_entities.size(); ++i)
			{
				m_entities[i]->DoTick(dt);
			}
		}
	}
}