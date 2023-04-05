#include "CollisionUtils.h"
#include "../Scene/Entity.h"

namespace Framework 
{
	namespace CollisionUtils 
	{
		int IntersectAxisAlignedBoxFrustum(const AxisAlignedBox* pVolumeA, const Frustum* pVolumeB) 
		{
			return XNA::IntersectAxisAlignedBoxFrustum(pVolumeA, pVolumeB);
		}

		void TransformFrustum(Frustum* pOut, const Frustum* pIn, FLOAT Scale, FXMVECTOR Rotation, FXMVECTOR Translation) 
		{
			XNA::TransformFrustum(pOut, pIn, Scale, Rotation, Translation);
		}

		void ComputeRendererWorldSpaceAxisAlignedBox(AxisAlignedBox* axisAlignedBoxW, Renderer* renderer)
		{
			XNA::AxisAlignedBox localAABB = renderer->mesh->GetAxisAlignedBox();
			Transform* transform = renderer->GetEntity()->GetTransform();
			XMMATRIX worldMat = transform->GetWorldMatrix();
			//
			XMFLOAT3 topPointL = localAABB.Center + localAABB.Extents;
			XMFLOAT3 bottomPointL = localAABB.Center + (localAABB.Extents * -1);
			//
			XMFLOAT3 bboxMaxW = { NAGETIVE_INFINITY,NAGETIVE_INFINITY,NAGETIVE_INFINITY };
			XMFLOAT3 bboxMinW = { POSITIVE_INFINITY,POSITIVE_INFINITY,POSITIVE_INFINITY };
			XMFLOAT3 bboxPointsL[2] = { topPointL ,bottomPointL };
			for (int i = 0; i < 2; ++i)
			{
				XMFLOAT3 pointW = bboxPointsL[i] * worldMat;
				bboxMaxW.x = pointW.x > bboxMaxW.x ? pointW.x : bboxMaxW.x;
				bboxMaxW.y = pointW.y > bboxMaxW.y ? pointW.y : bboxMaxW.y;
				bboxMaxW.z = pointW.z > bboxMaxW.z ? pointW.z : bboxMaxW.z;
				bboxMinW.x = pointW.x < bboxMinW.x ? pointW.x : bboxMinW.x;
				bboxMinW.y = pointW.y < bboxMinW.y ? pointW.y : bboxMinW.y;
				bboxMinW.z = pointW.z < bboxMinW.z ? pointW.z : bboxMinW.z;
			}
			//
			axisAlignedBoxW->Extents = (bboxMaxW - bboxMinW) * 0.5f;
			axisAlignedBoxW->Center = bboxMinW + axisAlignedBoxW->Extents;
		}

		bool IntersectRendererCamera(Renderer* renderer, Camera* camera) 
		{
			AxisAlignedBox aabbW;
			ComputeRendererWorldSpaceAxisAlignedBox(&aabbW, renderer);
			Frustum frustumW = camera->GetWorldSpaceFrustum();
			int intersect = XNA::IntersectAxisAlignedBoxFrustum(&aabbW, &frustumW);
			return intersect > 0;
		}
	}
}