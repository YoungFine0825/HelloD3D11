#pragma once

#include "../../math/MathLib.h"

struct Transform
{
	XMFLOAT3 position = { 0,0,0 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 scale = { 1,1,1 };
	//
	XMMATRIX GetWorldMatrix()
	{
		XMMATRIX trans = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX rot = XMMatrixRotationFromFloat3(rotation);
		XMMATRIX scaling = XMMatrixScaling(scale.x, scale.y, scale.z);
		return scaling * rot * trans;
	}
	//
	XMMATRIX GetLocalMatrix() 
	{
		return XMMatrixInverse(GetWorldMatrix());
	}
	//
	XMFLOAT3 GetWorldSpaceForward() 
	{
		XMFLOAT3 forward = { 0,0,1 };
		XMMATRIX rot = XMMatrixRotationFromFloat3(rotation);
		forward = XMFloat3MultiMatrix(forward, rot, 0);
		return XMVectorNormalize(forward);
	}
};