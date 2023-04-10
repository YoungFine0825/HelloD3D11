#pragma once
#include "../../math/MathLib.h"
#include "../../Color.h"
#include "Transform.h"
#include <vector>
#include <string>

namespace Framework 
{
	enum LIGHT_TYPE
	{
		LIGHT_TYPE_DIRECTIONAL = 0,
		LIGHT_TYPE_POINT,
		LIGHT_TYPE_SPOT,
	};

	typedef unsigned int LightInstanceId;

	class Light
	{
	public:
		Light();
		Light(LIGHT_TYPE type);
		~Light();
		//
		Light* SetName(std::string name);
		Light* SetEnable(bool enable);
		Light* SetType(LIGHT_TYPE type);
		Light* SetColor(RGBA32 color);
		Light* SetIntensity(float i);
		Light* SetRange(float range);
		Light* SetSpot(float spot);
		//
		LightInstanceId GetId();
		std::string GetName();
		bool IsEnabled();
		LIGHT_TYPE GetType();
		Transform* GetTransform();
		RGBA32 GetColor();
		XMFLOAT3 GetAttenuation();
		float GetIntensity();
		float GetRange();
		float GetSpot();
	private:
		LightInstanceId m_id;
		std::string m_name;
		bool m_enabled = true;
		LIGHT_TYPE m_type = LIGHT_TYPE_DIRECTIONAL;
		Transform m_transform;
		RGBA32 m_color;
		XMFLOAT3 m_attenuation = { 1,1,1 };
		float m_intensity = 0;
		float m_range = 1;
		float m_spot = 1;//聚光灯本影角（umbra angle）。单位：角度
	};

	typedef std::vector<Light*> LightVector;
}
