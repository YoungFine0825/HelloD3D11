#pragma once
#include "../../math/MathLib.h"

namespace Framework 
{
	class Mesh;
	namespace MeshUtil 
	{
		void ComputeMeshTangents(Mesh* mesh,UINT numVertices,UINT numIndices);
	}
}