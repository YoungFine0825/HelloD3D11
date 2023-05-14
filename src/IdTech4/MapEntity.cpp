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
		std::vector<float> values;
		_SplitValueToMultiFloats(value, values);
		if (values.size() < 3) 
		{
			return { 0,0,0 };
		}
		return { values[0],values[1],values[2] };
	}

	bool MapEntity::GetMatrix(const std::string& key, XMMATRIX* matrix)
	{
		std::string value;
		if (!_FindValue(key, value))
		{
			return false;
		}
		std::vector<float> values;
		if (!_SplitValueToMultiFloats(value, values)) 
		{
			return false;
		}
		size_t valueCount = values.size();
		bool success = true;
		//注意：MapFile里按行优先保存的列矩阵，并且需要交换Y轴和Z轴
		if (valueCount == 4)
		{
			matrix->_11 = values[0]; matrix->_12 = values[1];
			matrix->_21 = values[2]; matrix->_22 = values[3];
		}
		else if (valueCount == 9) 
		{
			matrix->_11 = values[0]; matrix->_12 = values[2]; matrix->_13 = values[1];
			matrix->_21 = values[6]; matrix->_22 = values[8]; matrix->_23 = values[7];
			matrix->_31 = values[3]; matrix->_32 = values[5]; matrix->_33 = values[4];
		}
		else if (valueCount == 16) 
		{
			matrix->_11 = values[0]; matrix->_12 = values[2]; matrix->_13 = values[1]; matrix->_13 = values[3];
			matrix->_21 = values[8]; matrix->_22 = values[10]; matrix->_23 = values[9]; matrix->_13 = values[7];
			matrix->_31 = values[4]; matrix->_32 = values[6]; matrix->_33 = values[5]; matrix->_13 = values[11];
			matrix->_41 = values[12]; matrix->_32 = values[13]; matrix->_33 = values[14]; matrix->_13 = values[15];
		}
		else 
		{
			success = false;
		}
		return success;
	}

	bool MapEntity::_SplitValueToMultiFloats(const std::string& value, std::vector<float>& floats) 
	{
		std::string token;
		std::istringstream ss(value);
		if (!ss) 
		{
			return false;
		}
		while (ss >> token)
		{
			floats.push_back(std::stof(token));
		}
		return true;
	}

	bool MapEntity::haveKey(const std::string& key) 
	{
		std::string value;
		return _FindValue(key, value);
	}
}