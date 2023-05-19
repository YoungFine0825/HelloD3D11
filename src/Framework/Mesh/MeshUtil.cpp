#include <vector>

#include "MeshUtil.h"

namespace Framework
{
	namespace MeshUtil
	{
		void ComputeTangents(XMFLOAT4* outTangents,
			XMFLOAT3* vertices, XMFLOAT3* normals, XMFLOAT2* texCoord, UINT numVertices,
			UINT* indices, UINT numIndices
		)
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
				UINT i0 = indices[t * 3 + 0];
				UINT i1 = indices[t * 3 + 1];
				UINT i2 = indices[t * 3 + 2];
				//�õ��������������������UV����
				XMFLOAT3 v0 = vertices[i0];
				XMFLOAT3 v1 = vertices[i1];
				XMFLOAT3 v2 = vertices[i2];
				XMFLOAT2 uv0 = texCoord[i0];
				XMFLOAT2 uv1 = texCoord[i1];
				XMFLOAT2 uv2 = texCoord[i2];
				//����������ƽ���ϣ�object space�����ߡ������ߵ��㷨����ο���
				// 1����Introduction to 3D Game Programming with DirectX 11�� Section 18.3
				// 2����Foundations of Game Engine Development Volume 2 Rendering��Section 7.5 Lists 7.4
				//����ֲ��ռ�������������������
				XMFLOAT3 e0 = v1 - v0;
				XMFLOAT3 e1 = v2 - v0;
				//���������UV�Ĺ���������ռ䣨���߿ռ䣩�������ε�����������
				XMFLOAT2 deltaUV0 = { uv1.x - uv0.x,uv1.y - uv0.y };
				XMFLOAT2 deltaUV1 = { uv2.x - uv0.x,uv2.y - uv0.y };
				//���ݹ�ʽ���ֲ��ռ��µı���������ͨ������TB����(���������븱����������ɵľ���)�õ��ֲ��ռ��µı�����
				//��ô���������ֲ��ռ��±�������ɵľ����������ռ��±�������ɵľ���������͵õ���TB����,Ҳ�Ϳ����õ������븱���ߡ�
				//��������"r"�����漰��2x2���������󣬾��忴�����ᵽ�������顣
				float r = 1 / (deltaUV0.x * deltaUV1.y - deltaUV0.y * deltaUV1.x);//��ĸ������ʵ����2x2���������ʽ
				XMFLOAT3 tangent =		(e0 * deltaUV1.y		- e1 * deltaUV0.y) * r;
				XMFLOAT3 bitangent =	(e0 * deltaUV1.x * -1	+ e1 * deltaUV0.x) * r;
				//�ۼ�ÿ����������������ƽ������ߺ͸�����
				tangents[i0] = tangents[i0] + tangent;
				tangents[i1] = tangents[i1] + tangent;
				tangents[i2] = tangents[i2] + tangent;
				bitangents[i0] = bitangents[i0] + bitangent;
				bitangents[i1] = bitangents[i1] + bitangent;
				bitangents[i2] = bitangents[i2] + bitangent;
			}
			//�Զ������ۼƵ�������������һ����orthonormalized��
			for (UINT i = 0; i < numVertices; ++i)
			{
				XMFLOAT3 t = tangents[i];
				XMFLOAT3 b = bitangents[i];
				XMFLOAT3 n = normals[i];
				XMFLOAT3 tangetWithoutHanded = XMVectorNormalize(XMVectorReject(t, n));//��һ��Reject������ȷ�����ߴ�ֱ�뷨�ߣ����Ҳ����ĳ���
				float tangentDir = XMVectorDot(XMVectorCross(t, b), n) > 0.0f ? 1.0f : -1.0f;//�ж����߳���
				outTangents[i].x = tangetWithoutHanded.x;
				outTangents[i].y = tangetWithoutHanded.y;
				outTangents[i].z = tangetWithoutHanded.z;
				outTangents[i].w = tangentDir;//���һλ�����������������ߵĳ���
			}
		}
	}
}