#include "math/MathLib.h"


XMFLOAT3 operator+(const XMFLOAT3& lhs, const XMFLOAT3& rhs) 
{
	return { lhs.x + rhs.x,lhs.y + rhs.y,lhs.z + rhs.z };
}

XMFLOAT3 operator-(const XMFLOAT3& lhs, const XMFLOAT3& rhs) 
{
	return { lhs.x - rhs.x,lhs.y - rhs.y,lhs.z - rhs.z };
}

XMFLOAT3 operator*(const XMFLOAT3& lhs, const float& rhs)
{
	return { lhs.x * rhs,lhs.y * rhs,lhs.z * rhs };
}

XMFLOAT3 operator*(const float& lhs, const XMFLOAT3& rhs) 
{
	return { rhs.x * lhs, rhs.y * lhs,rhs.z * lhs };
}

XMVECTOR XMVectorSet(XMFLOAT3 value)
{
	return XMVectorSet(value.x, value.y, value.z, 0.0f);
}

XMVECTOR XMVectorSet(XMFLOAT4 value) 
{
	return XMVectorSet(value.x, value.y, value.z, value.w);
}

XMFLOAT3 XMVectorSubtract(XMFLOAT3 r, XMFLOAT3 l) 
{
	XMFLOAT3 ret = { r.x - l.x,r.y - l.y,r.z - l.z };
	return ret;
}


XMFLOAT3 XMVectorRotationFromFloat3(XMFLOAT3 vec3, XMFLOAT3 rotation) 
{
	XMFLOAT3 ret;
	XMMATRIX rotMat = XMMatrixRotationFromFloat3(rotation);
	ret.x = vec3.x * rotMat._11 + vec3.y * rotMat._21 + vec3.z * rotMat._31;
	ret.y = vec3.x * rotMat._12 + vec3.y * rotMat._22 + vec3.z * rotMat._32;
	ret.z = vec3.x * rotMat._13 + vec3.y * rotMat._23 + vec3.z * rotMat._33;
	return ret;
}


XMMATRIX XMMatrixRotationFromFloat3(XMFLOAT3 angle3)
{
	XMMATRIX ret = XMMatrixRotationRollPitchYawFromVector(
		XMVectorSet(
			Angle2Radin(angle3.x),
			Angle2Radin(angle3.y),
			Angle2Radin(angle3.z),
			0.0f
		)
	);
	return ret;
}

XMFLOAT3 XMVectorCross(XMFLOAT3 a, XMFLOAT3 b)
{
	XMFLOAT3 ret;
	XMVECTOR c = XMVector3Cross(XMVectorSet(a.x, a.y, a.z, 0.0f), XMVectorSet(b.x, b.y, b.z, 0.0f));
	XMStoreFloat3(&ret, c);
	return ret;
}

XMFLOAT3 XMVectorNormalize(XMFLOAT3 v) 
{
	XMFLOAT3 ret;
	XMVECTOR c = XMVector3Normalize(XMVectorSet(v.x, v.y, v.z, 0.0f));
	XMStoreFloat3(&ret, c);
	return ret;
}

XMMATRIX XMMatrixInverse(XMMATRIX M) 
{
	XMVECTOR det;
	return XMMatrixInverse(&det, M);
}

XMMATRIX XMMatrixInverseTranspose(XMMATRIX M) 
{
	return XMMatrixTranspose(XMMatrixInverse(M));
}

XMMATRIX XMMatrixLookAtFromFloat3LH(XMFLOAT3 pos, XMFLOAT3 target, XMFLOAT3 up) 
{
	return XMMatrixLookAtLH(
		XMVectorSet(pos.x, pos.y, pos.z,1.0f), 
		XMVectorSet(target.x, target.y, target.z, 1.0f),
		XMVectorSet(up.x, up.y, up.z, 0.0f)
	);
}


XMMATRIX XMMatrixTranslationFromFloat3(XMFLOAT3 trans) 
{
	return XMMatrixTranslation(trans.x, trans.y, trans.z);
}



XMFLOAT3 XMFloat3MultiMatrix(XMFLOAT3 vector, XMMATRIX matrix) 
{
	XMFLOAT3 ret;
	ret.x = vector.x * matrix._11 + vector.y * matrix._21 + vector.z * matrix._31 + 1.0f * matrix._41;
	ret.y = vector.x * matrix._12 + vector.y * matrix._22 + vector.z * matrix._32 + 1.0f * matrix._42;
	ret.z = vector.x * matrix._13 + vector.y * matrix._23 + vector.z * matrix._33 + 1.0f * matrix._43;
	return ret;
}