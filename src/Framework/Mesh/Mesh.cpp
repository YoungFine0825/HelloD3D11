
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
		m_verticesNumber = 0;
		m_indicesNumber = 0;
		IResource::Release();
	}

	Mesh* Mesh::SetVertexData(UINT verticesNum, XMFLOAT3* vertexData)
	{
		ReleaseArrayPointer(m_vertices);
		//
		m_needRecreateVB = verticesNum != m_verticesNumber;
		m_verticesNumber = verticesNum;
		m_vertices = vertexData;
		SetDataDirty();
		return this;
	}
	Mesh* Mesh::SetIndexData(UINT indicesNum, UINT* indexData) 
	{
		ReleaseArrayPointer(m_indices);
		//
		m_needRecreateIB = indicesNum != m_indicesNumber;
		m_indicesNumber = indicesNum;
		m_indices = indexData;
		SetDataDirty();
		return this;
	}
	Mesh* Mesh::SetUVData(XMFLOAT2* uvData) 
	{
		ReleaseArrayPointer(m_uvs);
		//
		m_uvs = uvData;
		SetDataDirty();
		return this;
	}
	Mesh* Mesh::SetNormalData(XMFLOAT3* normalData) 
	{
		ReleaseArrayPointer(m_normals);
		//
		m_normals = normalData;
		SetDataDirty();
		return this;
	}
	Mesh* Mesh::SetColorData(XMFLOAT4* colorData) 
	{
		ReleaseArrayPointer(m_colors);
		//
		m_colors = colorData;
		SetDataDirty();
		return this;
	}
	Mesh* Mesh::SetTangentData(XMFLOAT3* tangentData) 
	{
		ReleaseArrayPointer(m_tangents);
		//
		m_tangents = tangentData;
		SetDataDirty();
		return this;
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
		if (m_vertices == nullptr) 
		{
			return Mesh_ErrCode_Invalid_Vertex_Data;
		}
		if (m_indices == nullptr) 
		{
			return Mesh_ErrCode_Invalid_Index_Data;
		}
		//
		if (m_needRecreateVB) { RecreateVertexBuffer(); m_needRecreateVB = false; }
		if (m_needRecreateIB) { RecreateIndexBuffer(); m_needRecreateIB = false; }
		//
		d3dGraphic::VertexFormat_Common* inputData = new d3dGraphic::VertexFormat_Common[m_verticesNumber]();
		bool haveNormal = m_normals != nullptr;
		bool haveUV = m_uvs != nullptr;
		bool haveColor = m_colors != nullptr;
		bool HaveTangent = m_tangents != nullptr;
		for (size_t i = 0; i < m_verticesNumber; ++i) 
		{
			inputData[i].position = m_vertices[i];
			if (haveNormal) 
			{
				inputData[i].normal = m_normals[i];
			}
			if (haveUV) 
			{
				inputData[i].textureCoord0 = m_uvs[i];
			}
			if (haveColor)
			{
				inputData[i].color = m_colors[i];
			}
			if (HaveTangent)
			{
				inputData[i].tangent = m_tangents[i];
			}
		}
		d3dGraphic::GetDeviceContext()->UpdateSubresource(m_pVertexBuffer, 0, nullptr, inputData, 0, 0);
		d3dGraphic::GetDeviceContext()->UpdateSubresource(m_pIndexBuffer, 0, nullptr, m_indices, 0, 0);
		//
		delete[] inputData;
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
		if (!d3dGraphic::CreateVertexBuffer(&m_pVertexBuffer, nullptr, sizeof(d3dGraphic::VertexFormat_Common), m_verticesNumber, m_bufferUsage))
		{

		}
	}

	void Mesh::RecreateIndexBuffer() 
	{
		ReleaseCOM(m_pIndexBuffer);
		if (!d3dGraphic::CreateIndexBuffer(&m_pIndexBuffer, nullptr, sizeof(UINT), m_indicesNumber, m_bufferUsage))
		{

		}
	}

	void Mesh::DeleteAllData() 
	{
		ReleaseArrayPointer(m_vertices);
		ReleaseArrayPointer(m_indices);
		ReleaseArrayPointer(m_uvs);
		ReleaseArrayPointer(m_normals);
		ReleaseArrayPointer(m_colors);
		ReleaseArrayPointer(m_tangents);
	}
}