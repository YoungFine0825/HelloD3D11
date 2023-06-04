#pragma once

#include "d3d/d3dGraphic.h"
#include "../Color.h"

namespace Framework 
{
	class RenderTexture;
	class Shader;
	class Mesh;
	class Texture;
	namespace Graphics 
	{
		void ClearBackground(const RGBA32 bgColor);
		void ClearDepthStencil(UINT clearFlag = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
		void ClearRenderTexture(Framework::RenderTexture* rt, const RGBA32 bgColor = Colors::Black, UINT clearFlag = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
		void Present();
		void SetRenderTarget(Framework::RenderTexture* rt);
		void SetDepthStencil(Framework::RenderTexture* depthStencil);
		void DrawMesh(Mesh* mesh, Shader* shader, int pass = -1);
		void Blit(ID3D11ShaderResourceView* srv, RenderTexture* dst,Shader* customShader = nullptr,UINT pass = 0);
		void Blit(RenderTexture* src, RenderTexture* dst, Shader* customShader = nullptr, UINT pass = 0);
		void Blit(Texture* texture, RenderTexture* dst, Shader* customShader = nullptr, UINT pass = 0);
	}
}
