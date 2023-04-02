
#include "Camera.h"

namespace Framework
{
	Camera::Camera()
	{

	}

	Camera::Camera(std::string name)
	{
		m_name = name;
	}

	Camera::~Camera()
	{

	}

	Camera* Camera::SetEnable(bool enabled) 
	{
		m_isEnabled = enabled;
		return this;
	}

	bool Camera::IsEnabled() 
	{
		return m_isEnabled;
	}

	Camera* Camera::SetBackgroundColor(RGBA32 color) 
	{
		m_bgColor = color;
		return this;
	}

	Camera* Camera::SetClearFlag(CameraClearFlag flag) 
	{
		m_clearFlag = flag;
		return this;
	}

	Camera* Camera::SetAspectRatio(float ratio) 
	{
		m_aspectRatio = ratio;
		return this;
	}

	Camera* Camera::SetOrthographic(bool ortho) 
	{ 
		m_isOrtho = ortho;
		return this;
	}

	Camera* Camera::SetFov(float fov)
	{
		m_fov = fov;
		return this;
	}

	Camera* Camera::SetSize(float size)
	{
		m_size = size;
		return this;
	}

	Camera* Camera::SetNearClipDistance(float distance) 
	{
		m_nearClipDistance = distance;
		return this;
	}

	Camera* Camera::SetFarClipDistance(float distance) 
	{
		m_farClipDistance = distance;
		return this;
	}

	Camera* Camera::SetRenderTexture(RenderTexture* rt) 
	{
		m_renderTexture = rt;
		return this;
	}

	Transform* Camera::GetTransform() 
	{
		return &m_tranform;
	}

	std::string Camera::GetName() 
	{
		return m_name;
	}

	bool Camera::IsOrthographic() { return m_isOrtho; }
	RGBA32 Camera::GetBackgroundColor() { return m_bgColor; }
	CameraClearFlag Camera::GetClearFlag() { return m_clearFlag; }
	float Camera::GetAspectRatio() { return m_aspectRatio; }
	float Camera::GetFov() { return m_fov; }
	float Camera::GetSize() { return m_size; }
	float Camera::GetNearClipDistance() { return m_farClipDistance; }
	float Camera::GetFarClipDistance() { return m_farClipDistance; }


	XMMATRIX Camera::GetViewMatrix() 
	{
		XMMATRIX trans = XMMatrixTranslationFromFloat3(m_tranform.position);
		XMMATRIX rot = XMMatrixRotationFromFloat3(m_tranform.rotation);
		return XMMatrixInverse(rot * trans);
	}

	XMMATRIX Camera::GetProjectMatrix() 
	{
		if (m_isOrtho) 
		{
			return XMMatrixOrthographicLH(m_size, m_size, m_nearClipDistance, m_farClipDistance);
		}
		else 
		{
			return XMMatrixPerspectiveFovLH(Angle2Radin(m_fov), m_aspectRatio, m_nearClipDistance, m_farClipDistance);
		}
	}

	RenderTexture* Camera::GetRenderTexture() 
	{
		return m_renderTexture;
	}
}