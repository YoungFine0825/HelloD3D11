#include <algorithm>
#include "../../Global.h"
#include "../Collision/CollisionUtils.h"
#include "../Scene/Entity.h"
#include "../Scene/Transform.h"
#include "../Scene/Camera.h"
#include "../Shader/ShaderManager.h"
#include "../Shader/Shader.h"
#include "../RenderTexture/RenderTexture.h"
#include "../RenderTexture/RenderTextureManager.h"
#include "../Graphic.h"
#include "ParallelLightShadowMap.h"

namespace Framework 
{
	ParallelLightShadowMap::ParallelLightShadowMap() : ShadowMap() 
	{
		m_cascadeParts = new CascadePart[m_cascadeCnt];
	}

	ParallelLightShadowMap::~ParallelLightShadowMap()
	{
		ReleaseArrayPointer(m_cascadeParts);
		m_cascadeParts = nullptr;
	}

	ParallelLightShadowMap* ParallelLightShadowMap::SetNearestShadowDistance(float distance) 
	{
		m_nearestShadowDistance = distance;
		return this;
	}

	void ParallelLightShadowMap::OnCreateShadowMap(unsigned int size) 
	{
		RenderTextureManager::ReleaseRenderTexture(m_renderTexture);
		//
		float mapWid = static_cast<float>(size);
		float mapHei = static_cast<float>(size);
		m_depthMapWidth = mapWid * m_cascadeCnt;
		m_depthMapHeight = mapHei;
		//
		for (int i = 0; i < m_cascadeCnt; ++i) 
		{
			m_cascadeParts[i].m_viewport.TopLeftX = i * mapWid;
			m_cascadeParts[i].m_viewport.TopLeftY = 0;
			m_cascadeParts[i].m_viewport.Width = mapWid;
			m_cascadeParts[i].m_viewport.Height = mapHei;
			m_cascadeParts[i].m_viewport.MinDepth = 0.0f;
			m_cascadeParts[i].m_viewport.MaxDepth = 1.0f;
		}

		RenderTextureDesc rtDesc = { 0 };
		rtDesc.width = m_depthMapWidth;
		rtDesc.height = m_depthMapHeight;
		rtDesc.includeColor = false;
		rtDesc.includeDepthStencil = true;
		rtDesc.msaaCount = 0;
		rtDesc.includeColor = m_enableTransparentShadow;
		rtDesc.includeDepthStencil = true;
		if (m_enableTransparentShadow) 
		{
			rtDesc.colorFormat = DXGI_FORMAT_R32_FLOAT;
		}
		m_renderTexture = RenderTextureManager::CreateRenderTexture(&rtDesc);
	}

	void ParallelLightShadowMap::SetShaderParamters(Shader* shader) 
	{
		//if (m_enableTransparentShadow) 
		//{
		//	shader->SetShaderResourceView("g_parallelShadowMap", m_renderTexture->GetColorTextureSRV());
		//}
		//else 
		//{
		//	shader->SetShaderResourceView("g_parallelShadowMap", m_renderTexture->GetDepthTextureSRV());
		//}
		shader->SetShaderResourceView("g_parallelShadowMap", m_renderTexture->GetDepthTextureSRV());
		shader->SetFloat("g_CSMSize", static_cast<float>(m_size));
		shader->SetFloat("g_CSMLevels", static_cast<float>(m_cascadeCnt));
		shader->SetMatrix4x4("g_pCSMVP0", m_cascadeParts[0].m_viewProjectMatrix);
		shader->SetMatrix4x4("g_pCSMVP1", m_cascadeParts[1].m_viewProjectMatrix);
	}


	AxisAlignedBox ParallelLightShadowMap::ComputeFrustumAABB(float fov, float aspect, float nearDistance, float farDistance, XMMATRIX worldMatrix)
	{
		AxisAlignedBox ret;
		XMFLOAT3 vertices[8];
		CollisionUtils::ComputeFrustumVertices(vertices, fov, aspect, nearDistance, farDistance);
		CollisionUtils::ComputeAABBFromCorners(&ret, vertices, worldMatrix);
		return ret;
	}

	void ParallelLightShadowMap::BuildCascadeParts(Camera* camera, RendererVector* visibleRenderers)
	{
		bool isOrtho = camera->IsOrthographic();
		XMMATRIX l2w = camera->GetTransform()->GetWorldMatrix();
		float farestPos = camera->GetFarClipDistance();
		Frustum frustum;
		float fov = camera->GetFov();
		float aspect = camera->GetAspectRatio();
		int lastIdx = max(0, m_cascadeCnt - 1);
		for (size_t c = 0; c < lastIdx; ++c)
		{
			float nearDis = c == 0 ? 0 : pow(m_nearestShadowDistance, c);
			float farDis = min(pow(m_nearestShadowDistance, c + 1), farestPos);
			m_cascadeParts[c].m_aabbW = ComputeFrustumAABB(fov, aspect, nearDis, farDis, l2w);
		}
		//
		//m_cascadeParts[lastIdx].m_aabbW = m_wholeSceneAABBW;
	}

	void ParallelLightShadowMap::BuildViewProjectMartrix(XMFLOAT3 litRotationW, AxisAlignedBox* sceneAABBW, Camera* camera)
	{
		//
		XMFLOAT3 origin = { 0,0,0 };
		XMMATRIX litRotationMatrixW = XMMatrixRotationFromFloat3(litRotationW);
		XMFLOAT3 litDirW = XMVectorNormalize(XMFloat3MultiMatrix({ 0,0,1 }, litRotationMatrixW));
		XMMATRIX worldToLightSpace = XMMatrixInverse(litRotationMatrixW);
		//
		AxisAlignedBox wholeSceneAABBLitSpace;
		CollisionUtils::TransformAABB(sceneAABBW, worldToLightSpace, &wholeSceneAABBLitSpace);
		float wholeSceenWid, wholeSceneHei, wholeSceneDepth;
		CollisionUtils::ComputeAABBDismension(&wholeSceneAABBLitSpace, &wholeSceenWid, &wholeSceneHei, &wholeSceneDepth);
		//
		int lastCascadePartIdx = max(0, m_cascadeCnt - 1);
		//
		XMFLOAT3 litPosW = sceneAABBW->Center + litDirW * -1 * (wholeSceneDepth / 2.0f);
		XMMATRIX litWorldMat = litRotationMatrixW * XMMatrixTranslationFromFloat3(litPosW);
		XMMATRIX proj = XMMatrixOrthographicLH(wholeSceenWid, wholeSceneHei, 0.0f, wholeSceneDepth);
		m_cascadeParts[lastCascadePartIdx].m_viewProjectMatrix = XMMatrixInverse(litWorldMat) * proj;
		//
		for (int c = 0; c < lastCascadePartIdx; ++c) 
		{
			AxisAlignedBox aabbW = m_cascadeParts[c].m_aabbW;
			float wid, hei,depth;
			AxisAlignedBox aabbLitSpace;
			CollisionUtils::TransformAABB(&aabbW, worldToLightSpace, &aabbLitSpace);
			CollisionUtils::ComputeAABBDismension(&aabbLitSpace, &wid, &hei, &depth);
			litPosW = aabbW.Center + litDirW * -1 * (wholeSceneDepth / 2.0f);
			litWorldMat = litRotationMatrixW * XMMatrixTranslationFromFloat3(litPosW);
			proj = XMMatrixOrthographicLH(wid, hei, 0.0f, wholeSceneDepth);
			m_cascadeParts[c].m_viewProjectMatrix = XMMatrixInverse(litWorldMat) * proj;
		}
	}

	void ParallelLightShadowMap::PreRender(FrameData* frameData) 
	{
		ShadowMap::PreRender(frameData);
		//
		RendererVector* renderers = &frameData->renderers;
		size_t rendererCnt = renderers->size();
		if (rendererCnt <= 0) 
		{
			return;
		}
		XMFLOAT3 max = { NAGETIVE_INFINITY ,NAGETIVE_INFINITY ,NAGETIVE_INFINITY };
		XMFLOAT3 min = { POSITIVE_INFINITY,POSITIVE_INFINITY ,POSITIVE_INFINITY };
		XMFLOAT3 corners[8];
		for (size_t r = 0; r < rendererCnt; ++r)
		{
			Renderer* renderer = (*renderers)[r];
			if (renderer->GetMaterialInstance()->IsCastShadow())
			{
				XNA::AxisAlignedBox localAABB = renderer->GetMeshInstance()->GetAxisAlignedBox();
				Transform* transform = renderer->GetEntity()->GetTransform();
				XMMATRIX worldMat = transform->GetWorldMatrix();
				CollisionUtils::ComputeAABBCorners(&localAABB, corners);
				for (int i = 0; i < 8; ++i)
				{
					XMFLOAT3 pointW = corners[i] * worldMat;
					max.x = pointW.x > max.x ? pointW.x : max.x;
					max.y = pointW.y > max.y ? pointW.y : max.y;
					max.z = pointW.z > max.z ? pointW.z : max.z;
					min.x = pointW.x < min.x ? pointW.x : min.x;
					min.y = pointW.y < min.y ? pointW.y : min.y;
					min.z = pointW.z < min.z ? pointW.z : min.z;
				}
			}
		}
		//
		m_wholeSceneAABBW.Extents = (max - min) * 0.5f;
		m_wholeSceneAABBW.Center = min + m_wholeSceneAABBW.Extents;
	}

	void ParallelLightShadowMap::Update(Camera* camera, RendererVector* visibleRenderers, RendererVector* allRenderers, XMFLOAT3 litRotationW)
	{
		BuildCascadeParts(camera, visibleRenderers);
		//
		BuildViewProjectMartrix(litRotationW, &m_wholeSceneAABBW,camera);
		//
		//set render target with native api
		//ID3D11DeviceContext* dc = d3dGraphic::GetDeviceContext();
		//ID3D11RenderTargetView* renderTargets[1] = { 0 };
		//dc->OMSetRenderTargets(1, renderTargets, m_depthMapDSV);
		//dc->ClearDepthStencilView(m_depthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
		Clear();
		for (int i = 0; i < m_cascadeCnt; ++i) 
		{
			//dc->RSSetViewports(1, &m_cascadeParts[i].m_viewport);
			m_renderTexture->SetViewport(&m_cascadeParts[i].m_viewport);
			Graphics::SetRenderTarget(m_renderTexture);
			//DrawScene
			size_t rendererCnt = allRenderers->size();
			for (size_t r = 0; r < rendererCnt; ++r)
			{
				Renderer* renderer = (*allRenderers)[r];
				if (renderer->GetMaterialInstance()->IsCastShadow())
				{
					DrawRenderer(renderer, m_cascadeParts[i].m_viewProjectMatrix);
				}
			}
		}
		//let screen being render target
		Graphics::SetRenderTarget(nullptr);
	}

	void ParallelLightShadowMap::PostRender() 
	{
		ShadowMap::PostRender();
	}
}