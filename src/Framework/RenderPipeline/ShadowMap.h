#pragma once
#include "../Graphic.h"
#include "FrameData.h"

namespace Framework
{
	class Shader;
	class Renderer;
	class RenderTexture;

	class ShadowMap 
	{
	public:
		ShadowMap();
		virtual ~ShadowMap();
		//
		ShadowMap* SetSize(unsigned int size);
		//
		ID3D11DepthStencilView* GetDSV();
		ID3D11ShaderResourceView* GetSRV();
		unsigned int GetSize();
		void Clear();
		void EnableTransparentShadow(bool enable);
		//
		virtual void PreRender(FrameData* frameData);
		virtual void PostRender();
		virtual void SetShaderParamters(Shader* shader);

	protected:
		virtual void DrawRenderer(Renderer* renderer,XMMATRIX viewProjectMatrix);
		unsigned int m_width{0};
		unsigned int m_height{0};
		unsigned int m_size{ 0 };
		RenderTexture* m_renderTexture;
		Shader* m_genShadowMapShader;
		bool m_enableTransparentShadow{ false };
	protected:
		virtual void OnCreateShadowMap(unsigned int size);
	};
}