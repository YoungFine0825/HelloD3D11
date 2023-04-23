#pragma once
#include "../Graphic.h"

namespace Framework
{
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
		D3D11_VIEWPORT* GetViewport();
		XMMATRIX GetViewProjectMatrix();
		XMMATRIX GetViewProjectTextureMatrix();
		unsigned int GetSize();
	protected:
		unsigned int m_width{0};
		unsigned int m_height{0};
		unsigned int m_size{ 0 };
		ID3D11ShaderResourceView* m_depthMapSRV;
		ID3D11DepthStencilView* m_depthMapDSV;
		D3D11_VIEWPORT m_viewport;
		XMMATRIX m_viewProjectMatrix;
		XMMATRIX m_viewProjectTextureMatrix;
	};
}