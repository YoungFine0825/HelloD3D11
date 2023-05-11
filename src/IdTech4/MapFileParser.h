#pragma once
#include "MapFile.h"

namespace IdTech4 
{
	class MapFileParser {
	public:
		MapFileParser();
		~MapFileParser();
		static bool LoadAndParser();
	};
}