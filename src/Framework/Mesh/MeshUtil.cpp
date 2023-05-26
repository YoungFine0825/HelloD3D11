#include <vector>
#include "Mesh.h"
#include "MeshUtil.h"

namespace Framework
{
	namespace MeshUtil
	{
		void ComputeMeshTangents(Mesh* mesh, UINT numVertices, UINT numIndices)
		{
			UINT numTri = numIndices / 3;
			std::vector<XMFLOAT3 > tangents(numVertices);
			std::vector<XMFLOAT3 > bitangents(numVertices);
			for (UINT i = 0; i < numVertices; ++i)
			{
				tangents[i] = { 0,0,0 };
				bitangents[i] = { 0,0,0 };
			}
			//
			for (UINT t = 0; t < numTri; ++t)
			{
				UINT i0 = mesh->GetVertexIndicesValue(t * 3 + 0);
				UINT i1 = mesh->GetVertexIndicesValue(t * 3 + 1);
				UINT i2 = mesh->GetVertexIndicesValue(t * 3 + 2);
				MeshVertexDataPtr vertex0 = mesh->GetVertex(i0);
				MeshVertexDataPtr vertex1 = mesh->GetVertex(i1);
				MeshVertexDataPtr vertex2 = mesh->GetVertex(i2);
				//拿到三角形三个顶点坐标和UV坐标
				XMFLOAT3 v0 = { vertex0->x,vertex0->y,vertex0->z };
				XMFLOAT3 v1 = { vertex1->x,vertex1->y,vertex1->z };
				XMFLOAT3 v2 = { vertex2->x,vertex2->y,vertex2->z };
				XMFLOAT2 uv0 = { vertex0->u,vertex0->v };
				XMFLOAT2 uv1 = { vertex1->u,vertex1->v };
				XMFLOAT2 uv2 = { vertex2->u,vertex2->v };
				//计算三角形平面上（object space）切线、副切线的算法具体参考：
				// 1、《Introduction to 3D Game Programming with DirectX 11》 Section 18.3
				// 2、《Foundations of Game Engine Development Volume 2 Rendering》Section 7.5 Lists 7.4
				//计算局部空间下三角形两条边向量
				XMFLOAT3 e0 = v1 - v0;
				XMFLOAT3 e1 = v2 - v0;
				//三个顶点的UV的构成了纹理空间（切线空间）下三角形的两条边向量
				XMFLOAT2 deltaUV0 = { uv1.x - uv0.x,uv1.y - uv0.y };
				XMFLOAT2 deltaUV1 = { uv2.x - uv0.x,uv2.y - uv0.y };
				//根据公式，局部空间下的边向量可以通过乘以TB矩阵(切线向量与副切线向量组成的矩阵)得到局部空间下的边向量
				//那么反过来，局部空间下边向量组成的矩阵乘以纹理空间下边向量组成的矩阵的逆矩阵就得到了TB矩阵,也就可以拿到切线与副切线。
				//下面的这个"r"标量涉及求2x2矩阵的逆矩阵，具体看上面提到的两本书。
				float r = 1 / (deltaUV0.x * deltaUV1.y - deltaUV0.y * deltaUV1.x);//分母部分其实就是2x2矩阵的行列式
				XMFLOAT3 tangent =		(e0 * deltaUV1.y		- e1 * deltaUV0.y) * r;
				XMFLOAT3 bitangent =	(e0 * deltaUV1.x * -1	+ e1 * deltaUV0.x) * r;
				//累计每个顶点所在三角形平面的切线和副切线
				tangents[i0] = tangents[i0] + tangent;
				tangents[i1] = tangents[i1] + tangent;
				tangents[i2] = tangents[i2] + tangent;
				bitangents[i0] = bitangents[i0] + bitangent;
				bitangents[i1] = bitangents[i1] + bitangent;
				bitangents[i2] = bitangents[i2] + bitangent;
			}
			//对顶点上累计的切线做正交归一化（orthonormalized）
			for (UINT i = 0; i < numVertices; ++i)
			{
				MeshVertexDataPtr vertex = mesh->GetVertex(i);
				XMFLOAT3 t = tangents[i];
				XMFLOAT3 b = bitangents[i];
				XMFLOAT3 n = { vertex->nx,vertex->ny,vertex->nz };
				XMFLOAT3 tangetWithoutHanded = XMVectorNormalize(XMVectorReject(t, n));//做一次Reject操作，确保切线垂直与法线，并且不关心朝向。
				float tangentDir = XMVectorDot(XMVectorCross(t, b), n) > 0.0f ? 1.0f : -1.0f;//判断切线朝向
				vertex->tx = tangetWithoutHanded.x;
				vertex->ty = tangetWithoutHanded.y;
				vertex->tz = tangetWithoutHanded.z;
				vertex->tw = tangentDir;//最后一位，我们用来保存切线的朝向
			}
		}
	}
}