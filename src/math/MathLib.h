#pragma once

#include <d3d11.h>
#include <xnamath.h>

const float PI = 3.1415926535f;

#define Angle2Radin(x) x / 180.0f * PI

XMFLOAT3 operator+(const XMFLOAT3& lhs, const XMFLOAT3& rhs);
XMFLOAT3 operator-(const XMFLOAT3& lhs, const XMFLOAT3& rhs);
XMFLOAT3 operator*(const XMFLOAT3& lhs, const float& rhs);
XMFLOAT3 operator*(const float& lhs, const XMFLOAT3& rhs);

XMVECTOR XMVectorSet(XMFLOAT3 value);
XMVECTOR XMVectorSet(XMFLOAT4 value);
XMFLOAT3 XMVectorSubtract(XMFLOAT3 r, XMFLOAT3 l);
XMFLOAT3 XMVectorRotationFromFloat3(XMFLOAT3 vec3, XMFLOAT3 rotation);
XMFLOAT3 XMVectorCross(XMFLOAT3 a, XMFLOAT3 b);
XMFLOAT3 XMVectorNormalize(XMFLOAT3 v);

XMMATRIX XMMatrixRotationFromFloat3(XMFLOAT3 angle3);
XMMATRIX XMMatrixInverse(XMMATRIX M);
XMMATRIX XMMatrixInverseTranspose(XMMATRIX M);
XMMATRIX XMMatrixLookAtFromFloat3LH(XMFLOAT3 pos, XMFLOAT3 target, XMFLOAT3 up);
XMMATRIX XMMatrixTranslationFromFloat3(XMFLOAT3 trans);

XMFLOAT3 XMFloat3MultiMatrix(XMFLOAT3 float3, XMMATRIX matrix);
