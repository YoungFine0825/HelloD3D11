#pragma once
#include "Collision.h"
#include "../Scene/Camera.h"
#include "../RenderPipeline/Renderer.h"

namespace Framework 
{
	namespace CollisionUtils 
	{
		int IntersectAxisAlignedBoxFrustum(const AxisAlignedBox* pVolumeA, const Frustum* pVolumeB);
		void TransformFrustum(Frustum* pOut, const Frustum* pIn, FLOAT Scale, FXMVECTOR Rotation, FXMVECTOR Translation);
		void ComputeRendererWorldSpaceAxisAlignedBox(AxisAlignedBox* axisAlignedBoxW, Renderer* renderer);
		bool IntersectRendererCamera(Renderer* renderer,Camera* camera);
		void ComputeAABBCorners(const AxisAlignedBox* aabb, XMFLOAT3 array[8]);
	}
}