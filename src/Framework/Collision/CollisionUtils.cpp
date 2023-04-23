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
			XMFLOAT3 bboxMaxW = { NAGETIVE_INFINITY,NAGETIVE_INFINITY,NAGETIVE_INFINITY };
			XMFLOAT3 bboxMinW = { POSITIVE_INFINITY,POSITIVE_INFINITY,POSITIVE_INFINITY };
			static XMFLOAT3 corners[8];
			ComputeAABBCorners(&localAABB, corners);
			for (int i = 0; i < 8; ++i)
			{
				XMFLOAT3 pointW = corners[i] * worldMat;
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

		void ComputeAABBCorners(const AxisAlignedBox* aabb, XMFLOAT3 array[8])
		{
			XMFLOAT3 max = aabb->Center + aabb->Extents;
			XMFLOAT3 min = aabb->Center - aabb->Extents;
			float height = abs(max.y - min.y);
			float width = abs(max.x - min.x);
			float depth = abs(max.z - min.z);
			//
			array[0] = max;
			array[1] = max + XMFLOAT3{ -width,0,0 };
			array[2] = max + XMFLOAT3(-width, -height, 0);
			array[3] = max + XMFLOAT3{ 0,-height,0 };
			//
			array[4] = min + XMFLOAT3{ width,height,0 };
			array[5] = min + XMFLOAT3{0, height, 0};
			array[6] = min;
			array[7] = min + XMFLOAT3{ width,0,0 };
		}
	}
}