#pragma once

#include "d3d/d3dGraphic.h"
#include "../Color.h"
#include "Mesh/Mesh.h"
#include "Shader/Shader.h"
#include "RenderTexture/RenderTexture.h"

namespace Framework 
{

	namespace Graphics 
	{
		void ClearBackground(const RGBA32 bgColor);
		void ClearDepthStencil(UINT clearFlag = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
		void Present();
		void SetRenderTarget(Framework::RenderTexture* rt);
		void DrawMesh(Mesh* mesh, Shader* shader);
		void Blit(Framework::RenderTexture* src, Framework::RenderTexture* dst);
	}
}
