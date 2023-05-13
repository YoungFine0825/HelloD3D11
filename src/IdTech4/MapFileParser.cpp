#include <iostream>
#include "MapFileParser.h"

#define KEYWORD_MODEL "model"
#define KEYWORD_NAME "name"
#define KEYWROD_NUM_SURFACE "numSurfaces"
#define KEYWORD_SURFACE "surface"
#define KEYWORD_NUM_VERTICES "numVerts"
#define KEYWORD_NUM_INDICES "numIndexes"
#define KEYWORD_LEFT_CURLY_BRACKET "{"
#define KEYWORD_RIGHT_CURLY_BRACKET "}"
#define KEYWORD_ENTITY "entity"
#define KEYWORD_PRIMITIVE "primitive"
#define KEYWORD_DOUBLE_SLASH "//"
#define KEYWORD_DOUBLE_QUOTATION '\"'

namespace IdTech4 
{
	static std::string token;

	MapFileParser::MapFileParser() 
	{

	}

	MapFileParser::~MapFileParser() 
	{

	}

	bool MapFileParser::LoadAndParser(const std::string& path, MapFilePtr mapFile)
	{
		size_t theLastDot = path.find_first_of('.', 0);
		std::string pathWithoutExt = path.substr(0, theLastDot);
		std::string procFilePath = pathWithoutExt + ".proc";
		std::string mapFilePath = pathWithoutExt + ".map";
		bool procFileParsed = ParseProcFile(procFilePath, mapFile);
		bool mapFileParsed = ParseMapFile(mapFilePath, mapFile);
		return procFileParsed || mapFileParsed;
	}

	void MapFileParser::ParseSurfaceVertexData(std::fstream& fs, MapModelSurfacePtr surface)
	{
		fs >> token;// (
		float value[8] = { 0,0,0,0,0,0,0,0 };
		for (int i = 0; i < 8; ++i)
		{
			fs >> token;
			value[i] = std::stof(token);
		}
		fs >> token;// )
		MapModelSurfaceVertex vertex;
		vertex.x = value[0];
		vertex.y = value[1];
		vertex.z = value[2];
		vertex.u = value[3];
		vertex.v = value[4];
		vertex.nx = value[5];
		vertex.ny = value[6];
		vertex.nz = value[7];
		surface->AddVertex(vertex);
	}

	void MapFileParser::ParseSurfaceData(std::fstream& fs, MapModelPtr model)
	{
		fs >> token;// /*
		fs >> token;// surface
		//
		fs >> token;
		int index = std::stoi(token);
		//
		fs >> token;// */
		fs >> token;// {
		fs >> token;
		std::string materialUrl = token.substr(1, token.length() - 2);
		fs >> token;// /*
		fs >> token;// numVerts
		fs >> token;// = 
		fs >> token;// */
		fs >> token;
		int numVertices = std::stoi(token);
		fs >> token;// /*
		fs >> token;// numIndexes
		fs >> token;// = 
		fs >> token;// */
		fs >> token;
		int numIndices = std::stoi(token);
		//
		MapModelSurfacePtr surface = model->CreateSurface(materialUrl,numVertices,numIndices);
		for (int v = 0; v < numVertices; ++v)
		{
			ParseSurfaceVertexData(fs, surface);
		}
		//
		for (int i = 0; i < numIndices; ++i)
		{
			fs >> token;
			surface->AddIndex(std::stoi(token));
		}
		fs >> token;// }
	}

	void MapFileParser::ParseModelData(std::fstream& fs, MapFilePtr mapFile)
	{
		fs >> token;// {
		fs >> token;// /*
		fs >> token;// name
		fs >> token;// =
		fs >> token;// */ 
		fs >> token;// 
		std::string name = token.substr(1, token.length() - 2);
		//
		fs >> token;// /*
		fs >> token;// numSurfaces
		fs >> token;//  =
		fs >> token;// */
		fs >> token;
		int numSurfaces = std::stoi(token);
		//
		MapModelPtr m = mapFile->CreateModel(name,numSurfaces);
		for (int s = 0; s < numSurfaces; ++s)
		{
			ParseSurfaceData(fs, m);
		}
		fs >> token;// }
	}

	bool MapFileParser::ParseProcFile(const std::string& path, MapFilePtr mapFile)
	{
		std::fstream file(path);
		if (!file)
		{
			std::cout << "[ParseProcFile] open proc file failed !" << std::endl;
			return false;
		}
		//
		std::string version;
		file >> version;
		std::cout << "[ParseProcFile] file version:" << version << std::endl;
		//
		while (file >> token)
		{
			if (token == KEYWORD_MODEL)
			{
				ParseModelData(file, mapFile);
			}
		}
		return true;
	}


	void MapFileParser::ParseEntityPrimitive(std::fstream& fs, MapEntity* ent)
	{
		fs >> token;//primitive
		fs >> token;// pri index
		fs >> token;// {
		fs >> token;// brushDef3 or patchDef3
		fs >> token;// {
		while (fs >> token)
		{
			if (token == KEYWORD_RIGHT_CURLY_BRACKET)
			{
				break;
			}
		}
		fs >> token;// }
	}

	void MapFileParser::ParseValue(std::fstream& fs, std::string& value)
	{
		fs >> value;
		if (value[value.length() - 1] == KEYWORD_DOUBLE_QUOTATION)
		{
			value = value.substr(1, value.length() - 2);
			return;
		}
		else
		{
			std::ostringstream out;
			out << value;
			while (fs >> value)
			{
				out << ' ' << value;
				if (value[value.length() - 1] == KEYWORD_DOUBLE_QUOTATION)
				{
					break;
				}
			}
			std::string realValue = out.str();
			value = realValue.substr(1, realValue.length() - 2);
		}
	}

	void MapFileParser::CheckEntityKey(std::fstream& fs, std::string& token, std::string& key)
	{
		if (token[token.length() - 1] == KEYWORD_DOUBLE_QUOTATION)
		{
			key = token.substr(1, token.length() - 2);
		}
		else
		{
			std::ostringstream out;
			out << token;
			while (fs >> token)
			{
				out << ' ' << token;
				if (token[token.length() - 1] == KEYWORD_DOUBLE_QUOTATION)
				{
					break;
				}
			}
			std::string realKey = out.str();
			key = realKey.substr(1, realKey.length() - 2);
		}
	}

	void MapFileParser::ParseEntityData(std::fstream& fs, MapFilePtr mapFile, int index)
	{
		fs >> token;// {
		MapEntity* ent = mapFile->CreateEntity();
		while (fs >> token)
		{
			if (token == KEYWORD_RIGHT_CURLY_BRACKET)
			{
				break;
			}
			else if (token == KEYWORD_DOUBLE_SLASH)
			{
				ParseEntityPrimitive(fs, ent);
			}
			else
			{
				std::string key;
				std::string value;
				CheckEntityKey(fs, token, key);
				ParseValue(fs, value);
				ent->AddKeyValue(key, value);
			}
		}
	}

	bool MapFileParser::ParseMapFile(const std::string& path, MapFilePtr mapFile)
	{
		std::fstream file(path);
		if (!file)
		{
			std::cout << "open map file failed :" << path << std::endl;
			return false;
		}
		file >> token;// Version
		file >> token;
		int version = std::stoi(token);
		int entityIndex = 0;
		while (file >> token)
		{
			if (token == KEYWORD_ENTITY)
			{
				file >> token;
				entityIndex = std::stoi(token);
				if (entityIndex > 0)
				{
					ParseEntityData(file, mapFile, entityIndex);
				}
			}
		}
		return true;
	}
}