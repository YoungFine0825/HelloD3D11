#pragma once

#include "d3d/d3dGraphic.h"
#include "../Resource.h"
#include "../../math/MathLib.h"
#include "../Collision/xnacollision.h"

namespace Framework 
{
	enum Mesh_ErrCode
	{
		Mesh_ErrCode_None = 0,
		Mesh_ErrCode_Invalid_Vertex_Data = 1,
		Mesh_ErrCode_Invalid_Index_Data = 2,
	};
	class Mesh : public Resource::IResource
	{
	public:
		Mesh();
		~Mesh() override;
		//
		void Release() override;
		//
		Mesh* SetVertexData(UINT verticesNum, XMFLOAT3* vertexData);
		Mesh* SetIndexData(UINT indicesNum, UINT* indexData);
		Mesh* SetUVData(XMFLOAT2* uvData);
		Mesh* SetNormalData(XMFLOAT3* normalData);
		Mesh* SetColorData(XMFLOAT4* colorData);
		Mesh* SetTangentData(XMFLOAT4* tangentData);
		Mesh* SetBoundingShape(XNA::AxisAlignedBox aabb);
		//
		UINT GetVerticesNumber() { return m_verticesNumber; }
		UINT GetIndicesNumber() { return m_indicesNumber; }
		//
		Mesh_ErrCode UpLoad();
		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }
		D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() { return m_topology; }
		//
		bool EnabledCPUReadWrite() { return m_isEnableCPUReadWrite; }
		XNA::AxisAlignedBox GetAxisAlignedBox();
	private:
		UINT m_verticesNumber = 0;
		UINT m_indicesNumber = 0;
		UINT m_inputLayoutIndex = 0;
		//
		XMFLOAT3* m_vertices;
		UINT* m_indices;
		XMFLOAT2* m_uvs;
		XMFLOAT3* m_normals;
		XMFLOAT4* m_colors;
		XMFLOAT4* m_tangents;
		//
		bool m_isDataDirty;
		void SetDataDirty();
		//
		D3D11_USAGE m_bufferUsage = D3D11_USAGE_DEFAULT;
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
		//
		bool m_needRecreateVB = true;
		bool m_needRecreateIB = true;
		void RecreateVertexBuffer();
		void RecreateIndexBuffer();
		//
		bool m_isEnableCPUReadWrite = false;
		void DeleteAllData();
		//
		D3D_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		//
		XNA::AxisAlignedBox m_aabb;
	};
}
