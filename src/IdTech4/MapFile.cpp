#include "MapFile.h"

namespace IdTech4 
{
	MapFile::MapFile() 
	{

	}

	MapFile::~MapFile() 
	{
		MapModelPtrVec::iterator m;
		MapModelPtrVec::iterator mend = m_models.end();
		for (m = m_models.begin(); m != mend; ++m)
		{
			delete (*m);
		}
		m_models.clear();
		//
		MapEntityPtrVec::iterator e;
		MapEntityPtrVec::iterator eend = m_entities.end();
		for (e = m_entities.begin(); e != eend; ++e)
		{
			delete (*e);
		}
		m_entities.clear();
	}

	MapModelPtr MapFile::CreateModel() 
	{
		MapModelPtr m = new MapModel();
		m_models.push_back(m);
		return m;
	}

	MapModelPtr MapFile::CreateModel(std::string name, int numSurfaces) 
	{
		MapModelPtr m = new MapModel(name, numSurfaces);
		m_models.push_back(m);
		return m;
	}

	MapModelPtrVec* MapFile::GetModels() 
	{
		return &m_models;
	}

	MapEntityPtr MapFile::CreateEntity()
	{
		MapEntityPtr e = new MapEntity();
		m_entities.push_back(e);
		return e;
	}

	MapEntityPtrVec* MapFile::GetEntities()
	{
		return &m_entities;
	}
}