#pragma once
#include <fstream>
#include <sstream>
#include "MapFile.h"

namespace IdTech4 
{
	class MapFileParser {
	public:
		MapFileParser();
		~MapFileParser();
		static bool LoadAndParser(const std::string& path, MapFilePtr mapFile);
	private:
		static void ParseSurfaceVertexData(std::fstream& fs, MapModelSurfacePtr surface);
		static void ParseSurfaceData(std::fstream& fs, MapModelPtr model);
		static void ParseModelData(std::fstream& fs, MapFilePtr mapFile);
		static bool ParseProcFile(const std::string& path, MapFilePtr mapFile);

		static void ParseEntityPrimitive(std::fstream& fs, MapEntity* ent);
		static void ParseValue(std::fstream& fs, std::string& value);
		static void CheckEntityKey(std::fstream& fs, std::string& token, std::string& key);
		static void ParseEntityData(std::fstream& fs, MapFilePtr mapFile, int index);
		static bool ParseMapFile(const std::string& path, MapFilePtr mapFile);
	};
}