#pragma once
#include <string>
#include <vector>
#include <map>
#include "../math/MathLib.h"

namespace IdTech4 
{
	struct MapModelSurfaceVertex
	{
		float x{ 0 };
		float y{ 0 };
		float z{ 0 };
		float u{ 0 };
		float v{ 0 };
		float nx{ 0 };
		float ny{ 0 };
		float nz{ 0 };
	};

	typedef std::vector<MapModelSurfaceVertex> MapModelSurfaceVertexVec;
	typedef std::vector<unsigned int> MapModelSurfaceIndexVec;

	class MapModelSurface 
	{
	public:
		MapModelSurface();
		MapModelSurface(std::string materialUrl, int numVerts, int numIndices);
		~MapModelSurface();
		//
		void AddVertex(MapModelSurfaceVertex v);
		void AddIndex(unsigned int idx);
		//
		MapModelSurfaceVertexVec* GetVertices();
		MapModelSurfaceIndexVec* GetIndices();
		std::string GetMaterialUrl();
	private:
		MapModelSurfaceVertexVec m_vertices;
		MapModelSurfaceIndexVec m_indices;
		std::string m_materialUrl;
	};

	typedef MapModelSurface* MapModelSurfacePtr;
	typedef std::vector<MapModelSurfacePtr> MapModelSurfacePtrVec;

	class MapModel 
	{
	public:
		MapModel();
		MapModel(std::string name,int numSurfaces);
		~MapModel();
		//
		void SetName(std::string name);
		std::string GetName();
		MapModelSurfacePtr CreateSurface();
		MapModelSurfacePtr CreateSurface(std::string materialUrl, int numVerts, int numIndices);
		void AddSurface(MapModelSurfacePtr surface);
		int GetNumSurfaces();
		MapModelSurfacePtrVec* GetSurfaces();
	private:
		MapModelSurfacePtrVec m_surface;
		std::string m_name;
	};
	typedef MapModel* MapModelPtr;
	typedef std::vector<MapModelPtr> MapModelPtrVec;

	typedef std::map<std::string, std::string> MapEntityValuePairs;

	class MapEntity {
	public:
		MapEntity();
		~MapEntity();
		//
		MapEntity* AddKeyValue(std::string key, std::string value);
		//
		std::string GetName();
		std::string GetClassName();
		XMFLOAT3 GetOrigin();
		//
		float GetFloat(const std::string& key);
		bool GetBool(const std::string& key);
		int GetInt(const std::string& key);
		std::string GetString(const std::string& key);
		XMFLOAT3 GetFloat3(const std::string& key);
		bool GetMatrix(const std::string& key,XMMATRIX* matrix);
		//
		bool haveKey(const std::string& key);
	private:
		bool _FindValue(const std::string key,std::string& out);
		int _IsMultiNumbers(const std::string value);
		bool _SplitValueToMultiFloats(const std::string& value, std::vector<float>& floats);
		MapEntityValuePairs m_values;
	};

	typedef MapEntity* MapEntityPtr;
	typedef std::vector<MapEntityPtr> MapEntityPtrVec;

	class MapFile {
	public:
		MapFile();
		~MapFile();
		//
		MapModelPtr CreateModel();
		MapModelPtr CreateModel(std::string name, int numSurfaces);
		MapModelPtrVec* GetModels();
		//
		MapEntityPtr CreateEntity();
		MapEntityPtrVec* GetEntities();
	private:
		MapModelPtrVec m_models;
		MapEntityPtrVec m_entities;
	};

	typedef MapFile* MapFilePtr;
}