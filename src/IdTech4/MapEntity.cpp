#include <sstream>
#include "MapFile.h"

namespace IdTech4 
{
	MapEntity::MapEntity() 
	{

	}

	MapEntity::~MapEntity() 
	{
		m_values.clear();
	}

	MapEntityPtr MapEntity::AddKeyValue(std::string key, std::string value) 
	{
		m_values[key] = value;
		return this;
	}

	std::string MapEntity::GetName() 
	{
		std::string name;
		_FindValue("name", name);
		return name;
	}

	std::string MapEntity::GetClassName() 
	{
		std::string className;
		_FindValue("classname", className);
		return className;
	}

	XMFLOAT3 MapEntity::GetOrigin() 
	{
		XMFLOAT3 origin = GetFloat3("origin");
		float y = origin.y;
		origin.y = origin.z;
		origin.z = y;
		return origin;
	}

	bool MapEntity::_FindValue(const std::string key, std::string& out)
	{
		MapEntityValuePairs::iterator it = m_values.find(key);
		if (it == m_values.end()) 
		{
			return false;
		}
		out = it->second;
		return true;
	}

	int MapEntity::_IsMultiNumbers(const std::string value)
	{
		int n = 0;
		char c = ' ';
		for (auto& a : value)
		{
			if (a == c)
			{
				n++;
			}
		}
		return ++n;
	}

	float MapEntity::GetFloat(const std::string& key)
	{
		std::string value;
		if (!_FindValue(key, value)) 
		{
			return 0;
		}
		int numberCnt = _IsMultiNumbers(value);
		if (numberCnt > 1) 
		{
			return 0;
		}
		float ret = std::stof(value);
		return ret;
	}

	bool MapEntity::GetBool(const std::string& key) 
	{
		std::string value;
		if (!_FindValue(key, value))
		{
			return false;
		}
		int numberCnt = _IsMultiNumbers(value);
		if (numberCnt > 1)
		{
			return false;
		}
		int ret = std::stoi(value);
		return ret == 1;
	}

	int MapEntity::GetInt(const std::string& key)
	{
		std::string value;
		if (!_FindValue(key, value))
		{
			return false;
		}
		int numberCnt = _IsMultiNumbers(value);
		if (numberCnt > 0)
		{
			return false;
		}
		int ret = std::stoi(value);
		return ret;
	}

	std::string MapEntity::GetString(const std::string& key)
	{
		std::string value = "";
		_FindValue(key, value);
		return value;
	}

	XMFLOAT3 MapEntity::GetFloat3(const std::string& key) 
	{
		std::string value;
		if (!_FindValue(key, value))
		{
			return { 0,0,0 };
		}
		std::string token;
		std::istringstream ss(value);
		float f3[3] = { 0,0,0 };
		int fIdx = 0;
		while (ss >> token) 
		{
			f3[fIdx] = std::stof(token);
			++fIdx;
			if (fIdx >= 3) { break; }
		}
		return { f3[0],f3[1],f3[2] };
	}
}