#pragma once
#include "Collision.h"
#include "../Scene/Camera.h"
#include "../RenderPipeline/Renderer.h"

namespace Framework 
{
	class Light;
	namespace CollisionUtils 
	{
		int IntersectAxisAlignedBoxFrustum(const AxisAlignedBox* pVolumeA, const Frustum* pVolumeB);
		void TransformFrustum(Frustum* pOut, const Frustum* pIn, FLOAT Scale, FXMVECTOR Rotation, FXMVECTOR Translation);
		void ComputeRendererWorldSpaceAxisAlignedBox(AxisAlignedBox* axisAlignedBoxW, Renderer* renderer);
		bool IntersectRendererCamera(Renderer* renderer,Camera* camera);
		void ComputeAABBCorners(const AxisAlignedBox* aabb, XMFLOAT3 array[8]);
		void ComputeAABBFromCorners(AxisAlignedBox* aabb, XMFLOAT3 corners[8],XMMATRIX trans = XMMatrixIdentity());
		void ComputeAABBDismension(const AxisAlignedBox* aabb, float* width, float* height, float* depth);
		void ComputeWorldSpaceFrustumFromProj(Frustum* pOut, XMMATRIX proj,XMMATRIX worldMatrix);
		void ComputeFrustumVertices(XMFLOAT3 array[8], float fov, float aspect, float nearDistance, float farDistance);
		void TransformAABB(const AxisAlignedBox* aabbIn, const XMMATRIX transform, AxisAlignedBox* aabbOut);
		//

		bool IntersectLightFrustum(Light* lit, const Frustum* frustum);
	}
}