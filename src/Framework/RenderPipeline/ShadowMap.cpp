#include "ShadowMap.h"
#include "../../Global.h"

namespace Framework
{

	ShadowMap::ShadowMap()
	{

	}

	ShadowMap::~ShadowMap()
	{
		ReleaseCOM(m_depthMapDSV);
		ReleaseCOM(m_depthMapSRV);
	}

	ShadowMap* ShadowMap::SetSize(unsigned int size)
	{
		if (m_size == size)
		{
			return this;
		}
		m_size = size;
		OnCreateShadowMap(size);
		return this;
	}

	ID3D11DepthStencilView* ShadowMap::GetDSV()
	{
		return m_depthMapDSV;
	}

	ID3D11ShaderResourceView* ShadowMap::GetSRV()
	{
		return m_depthMapSRV;
	}

	unsigned int ShadowMap::GetSize()
	{
		return m_size;
	}

	void ShadowMap::PreRender(FrameData* frameData)
	{

	}

	void ShadowMap::PostRender()
	{

	}

	void ShadowMap::SetShaderParamters(Shader* shader) 
	{

	}

	void ShadowMap::OnCreateShadowMap(unsigned int size)
	{

	}
}