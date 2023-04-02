#pragma once

#include "RenderTexture.h"

namespace Framework 
{
	namespace RenderTextureManager 
	{
		RenderTexture* FindRenderTexture(RenderTextureId id);
		void Cleanup();
		//
		RenderTexture* CreateRenderTexture(
			unsigned int width,
			unsigned int height,
			bool includeDepthStencil = true,
			bool createColorUAV = false,
			bool createDepthUAV = false,
			unsigned int msaaCount = 1
		);
		//
		void ReleaseRenderTexture(RenderTextureId id);
		void ReleaseRenderTexture(RenderTexture* rt);
	}
}
