#include "MapFile.h"

namespace IdTech4 
{
	MapModelSurface::MapModelSurface() 
	{

	}

	MapModelSurface::MapModelSurface(std::string materialUrl, int numVerts, int numIndices) :m_materialUrl(materialUrl)
	{
		m_vertices.reserve(numVerts);
		m_indices.reserve(numIndices);
	}

	MapModelSurface::~MapModelSurface() 
	{
		m_vertices.clear();
		m_indices.clear();
	}

	void MapModelSurface::AddVertex(MapModelSurfaceVertex v) 
	{
		m_vertices.push_back(v);
	}

	void MapModelSurface::AddIndex(unsigned int idx) 
	{
		m_indices.push_back(idx);
	}

	MapModelSurfaceVertexVec* MapModelSurface::GetVertices() 
	{
		return &m_vertices;
	}

	MapModelSurfaceIndexVec* MapModelSurface::GetIndices() 
	{
		return &m_indices;
	}

	std::string MapModelSurface::GetMaterialUrl() 
	{
		return m_materialUrl;
	}
}