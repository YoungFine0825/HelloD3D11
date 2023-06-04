#include "CollisionUtils.h"
#include "../Scene/Entity.h"
#include "../Scene/Light.h"

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
			XNA::AxisAlignedBox localAABB = renderer->GetMeshInstance()->GetAxisAlignedBox();
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

		void ComputeAABBFromCorners(AxisAlignedBox* aabb, XMFLOAT3 corners[8], XMMATRIX trans) 
		{
			XMFLOAT3 max = { NAGETIVE_INFINITY ,NAGETIVE_INFINITY ,NAGETIVE_INFINITY };
			XMFLOAT3 min = { POSITIVE_INFINITY,POSITIVE_INFINITY ,POSITIVE_INFINITY };
			for (int i = 0; i < 8; ++i)
			{
				XMFLOAT3 pointW = corners[i] * trans;
				max.x = pointW.x > max.x ? pointW.x : max.x;
				max.y = pointW.y > max.y ? pointW.y : max.y;
				max.z = pointW.z > max.z ? pointW.z : max.z;
				min.x = pointW.x < min.x ? pointW.x : min.x;
				min.y = pointW.y < min.y ? pointW.y : min.y;
				min.z = pointW.z < min.z ? pointW.z : min.z;
			}
			aabb->Extents = (max - min) * 0.5f;
			aabb->Center = min + aabb->Extents;
		}

		void ComputeAABBDismension(const AxisAlignedBox* aabb, float* width, float* height, float* depth) 
		{
			XMFLOAT3 max = aabb->Center + aabb->Extents;
			XMFLOAT3 min = aabb->Center - aabb->Extents;
			if (width) 
			{
				(*width) = abs(max.x - min.x);
			}
			if (height) 
			{
				(*height) = abs(max.y - min.y);
			}
			if (depth) 
			{
				(*depth) = abs(max.z - min.z);
			}
		}

		void ComputeWorldSpaceFrustumFromProj(Frustum* pOut, XMMATRIX proj, XMMATRIX worldMatrix)
		{
			Frustum frustumV;
			XNA::ComputeFrustumFromProjection(&frustumV, &proj);
			XMVECTOR scale;
			XMVECTOR rotQuat;
			XMVECTOR translation;
			XMMatrixDecompose(&scale, &rotQuat, &translation, worldMatrix);
			XNA::TransformFrustum(pOut, &frustumV, 1, rotQuat, translation);
		}

		void ComputeFrustumVertices(XMFLOAT3 array[8], float fov, float aspect, float nearDistance, float farDistance) 
		{
			float topF = farDistance * tan(Angle2Radin(fov) / 2.0f);
			float bottomF = topF * -1;
			float rightF = topF * aspect;
			float leftF = rightF * -1;
			//
			array[0] = { rightF,topF,nearDistance };
			array[1] = { leftF,topF,nearDistance };
			array[2] = { rightF,bottomF,nearDistance };
			array[3] = { leftF,bottomF,nearDistance };
			//
			array[4] = { rightF,topF,farDistance };
			array[5] = { leftF,topF,farDistance };
			array[6] = { rightF,bottomF,farDistance };
			array[7] = { leftF,bottomF,farDistance };
		}

		void TransformAABB(const AxisAlignedBox* aabbIn, const XMMATRIX transform, AxisAlignedBox* aabbOut) 
		{
			XMFLOAT3 corners[8];
			ComputeAABBCorners(aabbIn, corners);
			XMFLOAT3 max = { NAGETIVE_INFINITY ,NAGETIVE_INFINITY ,NAGETIVE_INFINITY };
			XMFLOAT3 min = { POSITIVE_INFINITY ,POSITIVE_INFINITY ,POSITIVE_INFINITY };
			for (int i = 0; i < 8; ++i)
			{
				XMFLOAT3 corner = XMFloat3MultiMatrix(corners[i], transform);
				max.x = corner.x > max.x ? corner.x : max.x;
				max.y = corner.y > max.y ? corner.y : max.y;
				max.z = corner.z > max.z ? corner.z : max.z;
				min.x = corner.x < min.x ? corner.x : min.x;
				min.y = corner.y < min.y ? corner.y : min.y;
				min.z = corner.z < min.z ? corner.z : min.z;
			}
			aabbOut->Extents = (max - min) * 0.5f;
			aabbOut->Center = min + aabbOut->Extents;
		}

		bool IntersectLightFrustum(Light* lit, const Frustum* frustum) 
		{
			LIGHT_TYPE type = lit->GetType();
			if (type == LIGHT_TYPE_DIRECTIONAL) { return true; }
			Transform* litTransform = lit->GetTransform();
			if (type == LIGHT_TYPE_POINT)
			{
				Sphere sphere;
				sphere.Center = litTransform->position;
				sphere.Radius = lit->GetRange();
				if (XNA::IntersectSphereFrustum(&sphere, frustum) > 0)
				{
					return true;
				}
			}
			else if (type == LIGHT_TYPE_SPOT)
			{
				float range = lit->GetRange();
				float radin = Angle2Radin(lit->GetSpot());
				float radius = tan(radin) * range;
				//为SpotLight构建一个朝向包围盒（OBB）,用OBB与renderer的AABB判断两者是否发碰撞
				OBB obbL;//先构建一个局部空间OBB
				obbL.Center = { 0,0,range / 2.0f };
				XMFLOAT3 endPoint = { 0,0,range };
				XMFLOAT3 max = { radius,radius,range };
				obbL.Extents = max - obbL.Center;
				obbL.Orientation = { 0,0,0,1 };
				//再应用光源的旋转和位移，将局部空间OBB转换到世界空间
				OBB obbW;
				XMVECTOR scale;
				XMVECTOR rotQuat;
				XMVECTOR translation;
				XMMATRIX litWorldMatrix = litTransform->GetWorldMatrix();
				XMMatrixDecompose(&scale, &rotQuat, &translation, litWorldMatrix);
				XNA::TransformOrientedBox(&obbW, &obbL, 1, rotQuat, translation);
				if (XNA::IntersectOrientedBoxFrustum(&obbW, frustum))
				{
					return true;
				}
			}
			return false;
		}
	}
}