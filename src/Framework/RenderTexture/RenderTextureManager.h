#pragma once

#include "RenderTexture.h"

namespace Framework 
{
	struct RenderTextureDesc 
	{
		unsigned int width = 0;
		unsigned int height = 0;
		bool includeColor = true;
		bool includeDepthStencil = true;
		bool createColorUAV = false;
		bool createDepthUAV = false;
		unsigned int msaaCount = 1;
	};

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
		RenderTexture* CreateRenderTexture(RenderTextureDesc* rtDesc);
		//
		void ReleaseRenderTexture(RenderTextureId id);
		void ReleaseRenderTexture(RenderTexture* rt);
	}
}
