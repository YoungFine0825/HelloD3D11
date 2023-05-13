#include "MapFile.h"

namespace IdTech4
{
	MapModel::MapModel()
	{

	}

	MapModel::MapModel(std::string name, int numSurfaces) : m_name(name)
	{
		m_surface.reserve(numSurfaces);
	}

	MapModel::~MapModel()
	{
		MapModelSurfacePtrVec::iterator it;
		MapModelSurfacePtrVec::iterator end = m_surface.end();
		for (it = m_surface.begin(); it != end; ++it) 
		{
			delete (*it);
		}
		m_surface.clear();
	}

	void MapModel::SetName(std::string name) 
	{
		m_name = name;
	}

	std::string MapModel::GetName() 
	{
		return m_name;
	}

	MapModelSurfacePtr MapModel::CreateSurface() 
	{
		MapModelSurfacePtr s = new MapModelSurface();
		m_surface.push_back(s);
		return s;
	}

	MapModelSurfacePtr MapModel::CreateSurface(std::string materialUrl, int numVerts, int numIndices) 
	{
		MapModelSurfacePtr s = new MapModelSurface(materialUrl, numVerts, numIndices);
		m_surface.push_back(s);
		return s;
	}

	void MapModel::AddSurface(MapModelSurfacePtr surface) 
	{
		m_surface.push_back(surface);
	}

	int MapModel::GetNumSurfaces() 
	{
		return m_surface.size();
	}

	MapModelSurfacePtrVec* MapModel::GetSurfaces() 
	{
		return &m_surface;
	}
}