#include "ShadowMap.h"
#include "../Shader/Shader.h"
#include "../Shader/ShaderManager.h"
#include "../RenderTexture/RenderTexture.h"
#include "../RenderTexture/RenderTextureManager.h"
#include "../Scene/Entity.h"
#include "../../Global.h"

namespace Framework
{

	ShadowMap::ShadowMap()
	{

	}

	ShadowMap::~ShadowMap()
	{
		RenderTextureManager::ReleaseRenderTexture(m_renderTexture);
		m_renderTexture = nullptr;
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
		if (!m_renderTexture) 
		{
			return nullptr;
		}
		return m_renderTexture->GetDSV();
	}

	ID3D11ShaderResourceView* ShadowMap::GetSRV()
	{
		if (!m_renderTexture)
		{
			return nullptr;
		}
		return m_renderTexture->GetDepthTextureSRV();
	}

	unsigned int ShadowMap::GetSize()
	{
		return m_size;
	}

	void ShadowMap::Clear()
	{
		if (!m_renderTexture) { return; }
		if (!m_renderTexture->GetDSV()) { return; }
		ID3D11DeviceContext* dc = d3dGraphic::GetDeviceContext();
		dc->ClearDepthStencilView(m_renderTexture->GetDSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void ShadowMap::EnableTransparentShadow(bool enable) 
	{
		m_enableTransparentShadow = enable;
	}

	void ShadowMap::PreRender(FrameData* frameData)
	{
		if (!m_genShadowMapShader) 
		{
			m_genShadowMapShader = ShaderManager::FindWithUrl("res/shaders/GenShadowMap.fx");
			if (!m_genShadowMapShader)
			{
				m_genShadowMapShader = ShaderManager::LoadFromFxFile("res/shaders/GenShadowMap.fx");
			}
		}
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

	void ShadowMap::DrawRenderer(Renderer* renderer, XMMATRIX viewProjectMatrix)
	{
		Entity* ent = renderer->GetEntity();
		Transform* trans = ent->GetTransform();
		XMMATRIX worldMat = trans->GetWorldMatrix();
		XMMATRIX mvp = worldMat * viewProjectMatrix;
		Material* matInst = renderer->GetMaterialInstance();
		Shader* shader = matInst->GetShader();
		int shadowCasterPassIndex = shader->GetPassIndex("ShadowCaster");
		if (shadowCasterPassIndex > -1)
		{
			shader->SetMatrix4x4("obj_MatMVP", mvp);
			matInst->Apply();
			Graphics::DrawMesh(renderer->GetMeshInstance(), shader, shadowCasterPassIndex);
		}
		else
		{
			m_genShadowMapShader->SetMatrix4x4("obj_MatMVP", mvp);
			Graphics::DrawMesh(renderer->GetMeshInstance(), m_genShadowMapShader);
		}
	}
}