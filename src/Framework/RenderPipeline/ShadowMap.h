#pragma once
#include "../Graphic.h"
#include "FrameData.h"

namespace Framework
{
	class Shader;
	class Renderer;

	class ShadowMap 
	{
	public:
		ShadowMap();
		~ShadowMap();
		//
		ShadowMap* SetSize(unsigned int size);
		//
		ID3D11DepthStencilView* GetDSV();
		ID3D11ShaderResourceView* GetSRV();
		unsigned int GetSize();
		void Clear();
		//
		virtual void PreRender(FrameData* frameData);
		virtual void PostRender();
		virtual void SetShaderParamters(Shader* shader);

	protected:
		virtual void DrawRenderer(Renderer* renderer,XMMATRIX viewProjectMatrix);
		unsigned int m_width{0};
		unsigned int m_height{0};
		unsigned int m_size{ 0 };
		ID3D11ShaderResourceView* m_depthMapSRV;
		ID3D11DepthStencilView* m_depthMapDSV;
		Shader* m_genShadowMapShader;
	protected:
		virtual void OnCreateShadowMap(unsigned int size);
	};
}