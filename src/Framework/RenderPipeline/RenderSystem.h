#pragma once
#include "RenderPipeline.h"
#include "../Scene/Camera.h"

namespace Framework 
{
	enum RenderSystemErrorCode
	{
		RENDER_SYSTEM_ERROR_NONE,
		RENDER_SYSTEM_ERROR_INIT_D3D_FAILED,
		RENDER_SYSTEM_ERROR_INVALID_PIPLINE,
		RENDER_SYSTEM_ERROR_INVALID_ARGS,
	};
	namespace RenderSystem 
	{
		RenderSystemErrorCode Init();
		void Cleanup();
		void SetRenderPipeline(RenderPipeline* pipline);
		RenderSystemErrorCode RenderFrame();
		RenderSystemErrorCode RenderCamera(Camera* camera);
	}
}