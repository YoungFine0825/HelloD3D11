#include <algorithm>
#include "../Graphic.h"
#include "../Material/Material.h"
#include "../Scene/Camera.h"
#include "../Scene/Entity.h"
#include "../Collision/CollisionUtils.h"
#include "FrameData.h"
#include "Renderer.h"
#include "RenderPipeline.h"


namespace Framework 
{
	RenderPipeline::RenderPipeline() 
	{

	}

	RenderPipeline::~RenderPipeline() 
	{

	}

	void RenderPipeline::DoRender(FrameData* frameData) 
	{
		m_frameData = frameData;
		//
		OnRender();
		//
		m_frameData = nullptr;
	}

	void RenderPipeline::SetDrawGUICallBack(RenderPipelineDrawGUICallback cb) 
	{
		m_drawGUICallBack = cb;
	}

	void RenderPipeline::SetDrawGizmosCallBack(RenderPipelineDrawGizmosCallback cb) 
	{
		m_drawGizmosCallBack = cb;
	}

	void RenderPipeline::OnRender() 
	{

	}

	void RenderPipeline::SetupCamera(Camera* camera)
	{
		Framework::RenderTexture* rt = camera->GetRenderTexture();
		Graphics::SetRenderTarget(rt);
		//
		CameraClearFlag clearFlag = camera->GetClearFlag();
		switch (clearFlag)
		{
		case Framework::CAMERA_CLEAR_SOLID_COLOR:
			Graphics::ClearBackground(camera->GetBackgroundColor());
			Graphics::ClearDepthStencil();
			break;
		case Framework::CAMERA_CLEAR_SKYBOX:
			Graphics::ClearBackground(camera->GetBackgroundColor());
			Graphics::ClearDepthStencil();
			break;
		case Framework::CAMERA_CLEAR_ONLY_DEPTHl:
			Graphics::ClearDepthStencil(D3D11_CLEAR_DEPTH);
			break;
		default:
			break;
		}
	}

	void RenderPipeline::CollectVisibleRenderers(Camera* camera, RendererVector* visibleRenderers)
	{
		Frustum worldSpaceFrustum = camera->GetWorldSpaceFrustum();
		RendererVector* enabledRenderers = &(m_frameData->renderers);
		size_t rendererCnt = enabledRenderers->size();
		for (size_t i = 0; i < rendererCnt; ++i)
		{
			Renderer* renderer = (*enabledRenderers)[i];
			if (renderer->GetMaterialInstance()->GetRenderQueue() == RENDER_QUEUE_BACKGROUND) 
			{
				visibleRenderers->push_back(renderer);
				continue;
			}
			AxisAlignedBox worldSpaceAABB;
			CollisionUtils::ComputeRendererWorldSpaceAxisAlignedBox(&worldSpaceAABB, renderer);
			int intersect = CollisionUtils::IntersectAxisAlignedBoxFrustum(&worldSpaceAABB, &worldSpaceFrustum);
			if (intersect > 0) 
			{
				visibleRenderers->push_back(renderer);
			}
		}
	}

	void RenderPipeline::SortVisibleRenderers(Camera* camera, RendererVector* visibleRenderers)
	{
		XMMATRIX viewMatrix = camera->GetViewMatrix();
		sort(visibleRenderers->begin(), visibleRenderers->end(),
			[viewMatrix](Renderer* a, Renderer* b)
			{
				MaterialRenderQueue queueA = a->GetMaterialInstance()->GetRenderQueue();
				MaterialRenderQueue queueB = b->GetMaterialInstance()->GetRenderQueue();
				if (queueA == queueB)
				{
					if (queueA == RENDER_QUEUE_TRANSPARENT)
					{
						XMFLOAT3 entPos = a->GetEntity()->GetTransform()->position;
						XMFLOAT3 posA = XMFloat3MultiMatrix(entPos, viewMatrix);
						//
						entPos = b->GetEntity()->GetTransform()->position;
						XMFLOAT3 posB = XMFloat3MultiMatrix(entPos, viewMatrix);
						//
						return posA.z > posB.z;
					}
					else
					{
						return a->GetEntity()->GetInstanceId() < b->GetEntity()->GetInstanceId();
					}
				}
				return queueA <= queueB;
			}
		);
	}
}