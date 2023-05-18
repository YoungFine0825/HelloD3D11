#pragma once
#include "../../math/MathLib.h"

namespace Framework 
{
	namespace MeshUtil 
	{
		void ComputeTangents(XMFLOAT4* outTangents, XMFLOAT3* vertices, XMFLOAT3* normals, XMFLOAT2* texCoord, UINT numVertices, UINT* indices, UINT numIndices);
	}
}