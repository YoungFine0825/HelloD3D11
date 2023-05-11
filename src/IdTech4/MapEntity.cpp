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

	MapEntityPtr MapEntity::SetName(std::string name) 
	{
		m_name = name;
		return this;
	}

	MapEntityPtr MapEntity::SetClassName(std::string className) 
	{
		m_className = className;
		return this;
	}

	MapEntityPtr MapEntity::SetOrigin(XMFLOAT3 origin)
	{
		m_origin = origin;
		return this;
	}

	MapEntityPtr MapEntity::AddKeyValue(std::string key, std::string value) 
	{
		m_values[key] = value;
		return this;
	}

	std::string MapEntity::GetName() 
	{
		return m_name;
	}

	std::string MapEntity::GetClassName() 
	{
		return m_className;
	}

	XMFLOAT3 MapEntity::GetOrigin() 
	{
		return m_origin;
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
		if (numberCnt > 0) 
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
		if (numberCnt > 0)
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
		std::vector<size_t> spaceCharPos;
		for (size_t c = 0; c < value.length(); ++c)
		{
			if (value[c] == ' ')
			{
				spaceCharPos.push_back(c);
			}
		}
		if (spaceCharPos.size() < 2)
		{
			return { 0,0,0 };
		}
		spaceCharPos.push_back(value.length());
		int offset = 0;
		std::vector<float> numbers;
		for (size_t p = 0; p < spaceCharPos.size(); ++p)
		{
			size_t len = spaceCharPos[p] - offset;
			std::string number = value.substr(offset, len);
			numbers.push_back(std::stof(number));
			offset = spaceCharPos[p];
		}
		return { numbers[0],numbers[1],numbers[2] };
	}
}