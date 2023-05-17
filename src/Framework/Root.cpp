#include "Root.h"
#include "../Window.h"
#include "../d3d/DXInput.h"
#include "../App.h"
#include "Shader/Shader.h"
#include "Texture/Texture.h"
#include "Mesh/Mesh.h"

namespace Framework 
{
	namespace Root 
	{
		bool Init() 
		{
			if (RenderSystem::Init() != RENDER_SYSTEM_ERROR_NONE) 
			{
				return false;
			}
			//
			dxinput_Init(win_GetInstance(), win_GetHandle());
			return true;
		}

		void Tick(float dt) 
		{
			dxinput_Tick();
			SceneManager::Tick(dt);
		}

		void Draw() 
		{
			RenderSystem::RenderFrame();
		}

		void Shutdown() 
		{
			SceneManager::Cleanup();
			ModelManager::Cleanup();
			MeshManager::Cleanup();
			ShaderManager::Cleanup();
			TextureManager::Cleanup();
			RenderTextureManager::Cleanup();
			dxinput_Cleanup();
			RenderSystem::Cleanup();
		}
	}
}