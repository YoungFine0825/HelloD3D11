#pragma once

#include "RenderPipeline/RenderSystem.h"
#include "Model/ModelManager.h"
#include "Shader/ShaderManager.h"
#include "Texture/TextureManager.h"
#include "Material/MaterialManager.h"
#include "Mesh/MeshManager.h"
#include "Scene/SceneManager.h"
#include "RenderTexture/RenderTextureManager.h"
#include "ImGUI/ImGuiHelper.h"

namespace Framework 
{
	namespace Root 
	{
		bool Init();
		void Tick(float dt);
		void Draw();
		void Shutdown();
	}
}