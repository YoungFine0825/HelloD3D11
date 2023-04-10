#pragma once

#include "../../math/MathLib.h"
#include "../../Color.h"

namespace Framework 
{
	struct ParallelLight
	{
		XMFLOAT4 color = {1.0f,1.0f,1.0f,1.0f};
		XMFLOAT3 directionW = {0,0,0};
		float intensity = 0;
	};

	struct PointLight
	{
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
		XMFLOAT3 positionW = { 0,0,0 };
		float intensity = 0;
		XMFLOAT3 attenuation = {1,1,1};
		float range = 1;
	};

	struct SpotLight
	{
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
		XMFLOAT3 positionW = { 0,0,0 };
		float intensity = 0;
		XMFLOAT3 attenuation = { 1,1,1 };
		float range = 1;
		XMFLOAT3 directionW = { 0,0,1 };
		float spot = 1;
	};
}