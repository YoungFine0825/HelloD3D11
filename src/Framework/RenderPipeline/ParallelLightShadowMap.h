#pragma once
#include "ShadowMap.h"
#include "ShaderStruct.h"
#include "../../math/MathLib.h"
#include "../Collision/Collision.h"
#include "Renderer.h"

namespace Framework 
{
	class ParallelLightShadowMap : public ShadowMap 
	{
	public:
		ParallelLightShadowMap();
		~ParallelLightShadowMap();
		//
		XMMATRIX BuildViewProjectMartrix(XMFLOAT3 litRotationW, AxisAlignedBox* sceneAABBW);
	};
}