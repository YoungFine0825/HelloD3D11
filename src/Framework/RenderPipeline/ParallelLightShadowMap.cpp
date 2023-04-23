#include "ParallelLightShadowMap.h"
#include "../Collision/CollisionUtils.h"

namespace Framework 
{
	ParallelLightShadowMap::ParallelLightShadowMap() : ShadowMap() 
	{

	}

	ParallelLightShadowMap::~ParallelLightShadowMap() 
	{
		ShadowMap::~ShadowMap();
	}

	XMMATRIX ParallelLightShadowMap::BuildViewProjectMartrix(XMFLOAT3 litRotationW, AxisAlignedBox* sceneAABBW)
	{
		XMMATRIX ret = XMMatrixIdentity();
		//
		XMFLOAT3 origin = { 0,0,0 };
		XMFLOAT3 litDirW = XMVectorNormalize(XMFloat3MultiMatrix({ 0,0,1 }, XMMatrixRotationFromFloat3(litRotationW)));
		XMMATRIX litWorldMat = XMMatrixRotationFromFloat3(litRotationW);
		XMMATRIX worldToLightSpace = XMMatrixInverse(litWorldMat);
		XMFLOAT3 maxLitSpace = { NAGETIVE_INFINITY ,NAGETIVE_INFINITY ,NAGETIVE_INFINITY };
		XMFLOAT3 minLitSpace = { POSITIVE_INFINITY ,POSITIVE_INFINITY ,POSITIVE_INFINITY };
		XMFLOAT3 cornersW[8];
		CollisionUtils::ComputeAABBCorners(sceneAABBW, cornersW);
		for (int i = 0; i < 8; ++i) 
		{
			XMFLOAT3 corner = XMFloat3MultiMatrix(cornersW[i], worldToLightSpace);
			maxLitSpace.x = corner.x > maxLitSpace.x ? corner.x : maxLitSpace.x;
			maxLitSpace.y = corner.y > maxLitSpace.y ? corner.y : maxLitSpace.y;
			maxLitSpace.z = corner.z > maxLitSpace.z ? corner.z : maxLitSpace.z;
			minLitSpace.x = corner.x < minLitSpace.x ? corner.x : minLitSpace.x;
			minLitSpace.y = corner.y < minLitSpace.y ? corner.y : minLitSpace.y;
			minLitSpace.z = corner.z < minLitSpace.z ? corner.z : minLitSpace.z;
		}
		//
		float sceneHeight = abs(maxLitSpace.y - minLitSpace.y);
		float sceneWidth = abs(maxLitSpace.x - minLitSpace.x);
		float sceneDepth = abs(maxLitSpace.z - minLitSpace.z);
		XMMATRIX proj = XMMatrixOrthographicLH(sceneWidth, sceneHeight, 1.0f, sceneDepth + 2.0f);
		//
		XMFLOAT3 litPosW = sceneAABBW->Center + litDirW * -1 * (sceneDepth / 2.0f + 1.0f);
		litWorldMat = XMMatrixRotationFromFloat3(litRotationW) * XMMatrixTranslationFromFloat3(litPosW);
		worldToLightSpace = XMMatrixInverse(litWorldMat);
		ret = worldToLightSpace * proj;
		//
		m_viewProjectMatrix = ret;
		m_viewProjectTextureMatrix = ret * NDC2TEXTURE;
		//
		return ret;
	}
}