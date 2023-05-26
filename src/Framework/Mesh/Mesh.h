#pragma once
#include <vector>
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

	struct MeshVertexData 
	{
		float x, y, z = 0;
		float nx, ny, nz = 0;
		float u, v = 0;
		float tx = 0, ty = 0, tz = 0, tw = 0 ;
		float r = 1.0f,g = 1.0f, b = 1.0f, a = 1.0f;
	};

	typedef UINT MeshVertexIndex;
	typedef std::vector<MeshVertexIndex> MeshIndicesDataVec;

	typedef MeshVertexData* MeshVertexDataPtr;
	typedef std::vector<MeshVertexData> MeshVerticesDataVec;

	class Mesh : public Resource::IResource
	{
	public:
		Mesh();
		~Mesh() override;
		//
		void Release() override;
		//
		Mesh* SetBoundingShape(XNA::AxisAlignedBox aabb);
		//
		UINT GetVerticesNumber();
		UINT GetIndicesNumber();
		//
		Mesh_ErrCode UpLoad();
		ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }
		D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() { return m_topology; }
		//
		bool EnabledCPUReadWrite() { return m_isEnableCPUReadWrite; }
		XNA::AxisAlignedBox GetAxisAlignedBox();
		//
		Mesh* ClearVertices();
		Mesh* ReserveVertices(UINT verticesNum);
		MeshVertexDataPtr GetVertex(UINT index);
		MeshVertexDataPtr CreateVertex();
		Mesh* AddVertex(MeshVertexData vertex);
		//
		Mesh* ClearVertexIndices();
		Mesh* ReserveVertexIndices(UINT indicesNum);
		Mesh* SetVertexIndicesValue(UINT index, MeshVertexIndex value);
		MeshVertexIndex GetVertexIndicesValue(UINT index);
		Mesh* AddVertexIndex(MeshVertexIndex idx);
	private:

		UINT m_inputLayoutIndex = 0;
		//
		MeshVerticesDataVec m_verticesData;
		MeshIndicesDataVec m_indicesData;
		UINT m_numVertices;
		UINT m_numIndices;
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
