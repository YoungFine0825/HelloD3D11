
#include "Mesh.h"

#include "../../Global.h"

namespace Framework 
{
	Mesh::Mesh() : IResource() 
	{
		m_type = Resource::RESOURCE_TYPE_MESH;
		m_inputLayoutIndex = D3D_INPUT_DESC_COMMON;
	}

	Mesh::~Mesh() 
	{

		IResource::~IResource();
	}

	void Mesh::Release()
	{
		ReleaseCOM(m_pVertexBuffer);
		ReleaseCOM(m_pIndexBuffer);
		DeleteAllData();
		IResource::Release();
	}

	Mesh* Mesh::SetBoundingShape(XNA::AxisAlignedBox aabb) 
	{
		m_aabb = aabb;
		return this;
	}

	XNA::AxisAlignedBox Mesh::GetAxisAlignedBox() 
	{
		return m_aabb;
	}

	void Mesh::SetDataDirty() 
	{
		m_isDataDirty = true;
	}

	Mesh_ErrCode Mesh::UpLoad()
	{
		if (!m_isDataDirty) 
		{
			return Mesh_ErrCode_None;
		}
		if (m_verticesData.size() <= 0) 
		{
			return Mesh_ErrCode_Invalid_Vertex_Data;
		}
		if (m_indicesData.size() <= 0)
		{
			return Mesh_ErrCode_Invalid_Index_Data;
		}
		//
		if (m_needRecreateVB) { RecreateVertexBuffer(); m_needRecreateVB = false; }
		if (m_needRecreateIB) { RecreateIndexBuffer(); m_needRecreateIB = false; }
		//
		size_t numVertices = m_verticesData.size();
		d3dGraphic::VertexFormat_Common* inputData = new d3dGraphic::VertexFormat_Common[numVertices]();
		for (size_t i = 0; i < numVertices; ++i)
		{
			MeshVertexDataPtr vertex = &m_verticesData[i];
			inputData[i].position.x = vertex->x;
			inputData[i].position.y = vertex->y;
			inputData[i].position.z = vertex->z;
			inputData[i].normal.x = vertex->nx;
			inputData[i].normal.y = vertex->ny;
			inputData[i].normal.z = vertex->nz;
			inputData[i].textureCoord0.x = vertex->u;
			inputData[i].textureCoord0.y = vertex->v;
			inputData[i].color.x = vertex->r;
			inputData[i].color.y = vertex->g;
			inputData[i].color.z = vertex->b;
			inputData[i].color.w = vertex->a;
			inputData[i].tangent.x = vertex->tx;
			inputData[i].tangent.y = vertex->ty;
			inputData[i].tangent.z = vertex->tz;
			inputData[i].tangent.w = vertex->tw;
		}
		size_t numIndices = m_indicesData.size();
		MeshVertexIndex* indices = new MeshVertexIndex[numIndices];
		for (size_t i = 0; i < numIndices; ++i) 
		{
			indices[i] = m_indicesData[i];
		}
		d3dGraphic::GetDeviceContext()->UpdateSubresource(m_pVertexBuffer, 0, nullptr, inputData, 0, 0);
		d3dGraphic::GetDeviceContext()->UpdateSubresource(m_pIndexBuffer, 0, nullptr, indices, 0, 0);
		//
		delete[] inputData;
		delete[] indices;
		//
		m_numVertices = numVertices;
		m_numIndices = numIndices;
		//
		if (!m_isEnableCPUReadWrite) 
		{
			DeleteAllData();
		}
		m_isDataDirty = false;
		return Mesh_ErrCode_None;
	}

	void Mesh::RecreateVertexBuffer() 
	{
		ReleaseCOM(m_pVertexBuffer);
		size_t numVertices = m_verticesData.size();
		if (!d3dGraphic::CreateVertexBuffer(&m_pVertexBuffer, nullptr, sizeof(d3dGraphic::VertexFormat_Common), numVertices, m_bufferUsage))
		{

		}
	}

	void Mesh::RecreateIndexBuffer() 
	{
		ReleaseCOM(m_pIndexBuffer);
		UINT numIndices = m_indicesData.size();
		if (!d3dGraphic::CreateIndexBuffer(&m_pIndexBuffer, nullptr, sizeof(UINT), numIndices, m_bufferUsage))
		{

		}
	}

	Mesh* Mesh::ClearVertices() 
	{
		m_verticesData.clear();
		m_isDataDirty = true;
		return this;
	}

	Mesh* Mesh::ReserveVertices(UINT verticesNum) 
	{
		m_verticesData.reserve(verticesNum);
		return this;
	}

	MeshVertexDataPtr Mesh::GetVertex(UINT index) 
	{
		size_t cnt = m_verticesData.size();
		if (index >= cnt)
		{
			return nullptr;
		}
		m_isDataDirty = true;
		MeshVertexDataPtr ret = &(m_verticesData[index]);
		return ret;
	}

	MeshVertexDataPtr Mesh::CreateVertex() 
	{
		MeshVertexData vertex;
		m_verticesData.push_back(vertex);
		MeshVertexDataPtr ret = &(m_verticesData[m_verticesData.size() - 1]);
		m_isDataDirty = true;
		return ret;
	}

	Mesh* Mesh::AddVertex(MeshVertexData vertex) 
	{
		m_verticesData.push_back(vertex);
		m_isDataDirty = true;
		return this;
	}

	Mesh* Mesh::ClearVertexIndices() 
	{
		m_indicesData.clear();
		m_isDataDirty = true;
		return this;
	}

	Mesh* Mesh::ReserveVertexIndices(UINT indicesNum)
	{
		m_indicesData.reserve(indicesNum);
		m_isDataDirty = true;
		return this;
	}

	Mesh* Mesh::SetVertexIndicesValue(UINT index, MeshVertexIndex value)
	{
		if (index < m_indicesData.size()) 
		{
			m_isDataDirty = true;
			m_indicesData[index] = value;
		}
		return this;
	}

	MeshVertexIndex Mesh::GetVertexIndicesValue(UINT index)
	{
		if (index < m_indicesData.size())
		{
			return m_indicesData[index];
		}
		return 0;
	}

	Mesh* Mesh::AddVertexIndex(MeshVertexIndex idx) 
	{
		m_indicesData.push_back(idx);
		m_isDataDirty = true;
		return this;
	}

	UINT Mesh::GetVerticesNumber() 
	{
		return m_numVertices;
	}

	UINT Mesh::GetIndicesNumber() 
	{
		return m_numIndices;
	}

	void Mesh::DeleteAllData() 
	{
		m_verticesData.clear();
		m_indicesData.clear();
		m_isDataDirty = true;
	}
}