
#include "Light.h"

namespace Framework 
{
	LightInstanceId g_lightInstanceId = 0;

	Light::Light() : m_id(g_lightInstanceId++)
	{
		m_name = "Light" + g_lightInstanceId;
	}

	Light::Light(LIGHT_TYPE type) :m_type(type) , m_id(g_lightInstanceId++)
	{
		m_name = "Light" + g_lightInstanceId;
	}

	Light::~Light() 
	{

	}

	Light* Light::SetName(std::string name) 
	{
		m_name = name;
		return this;
	}

	Light* Light::SetEnable(bool enable) 
	{
		m_enabled = enable;
		return this;
	}

	Light* Light::SetType(LIGHT_TYPE type) 
	{
		m_type = type;
		return this;
	}

	Light* Light::SetColor(RGBA32 color) 
	{
		m_color = color;
		return this;
	}

	Light* Light::SetIntensity(float i) 
	{
		m_intensity = i;
		return this;
	}

	Light* Light::SetRange(float range) 
	{
		m_range = range;
		return this;
	}

	Light* Light::SetSpot(float spot) 
	{
		m_spot = spot;
		return this;
	}

	LightInstanceId Light::GetId() 
	{
		return m_id;
	}

	std::string Light::GetName() 
	{
		return m_name;
	}

	bool Light::IsEnabled() 
	{
		return m_enabled;
	}

	LIGHT_TYPE Light::GetType() 
	{
		return m_type;
	}

	Transform* Light::GetTransform()
	{
		return &m_transform;
	}

	RGBA32 Light::GetColor() 
	{
		return m_color;
	}

	XMFLOAT3 Light::GetAttenuation() 
	{
		return m_attenuation;
	}

	float Light::GetIntensity() 
	{
		return m_intensity;
	}

	float Light::GetRange() 
	{
		return m_range;
	}

	float Light::GetSpot() 
	{
		return m_spot;
	}
}