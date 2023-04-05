#pragma once
#include "../../math/MathLib.h"
#include "../../Color.h"
#include "Transform.h"
#include <string>
#include "../RenderTexture/RenderTexture.h"
#include "../Collision/Collision.h"

namespace Framework 
{
	enum CameraClearFlag
	{
		CAMERA_CLEAR_SOLID_COLOR,
		CAMERA_CLEAR_SKYBOX,
		CAMERA_CLEAR_ONLY_DEPTHl,
		CAMERA_CLEAR_DONT_CLEAR,
	};

	class Camera 
	{
	public:
		Camera();
		Camera(std::string name);
		~Camera();

		Transform* GetTransform();
		Camera* SetEnable(bool enabled);
		bool IsEnabled();

		Camera* SetBackgroundColor(RGBA32 color);
		Camera* SetClearFlag(CameraClearFlag flag);
		Camera* SetAspectRatio(float ratio);
		Camera* SetOrthographic(bool ortho);
		Camera* SetFov(float fov);
		Camera* SetSize(float size);
		Camera* SetNearClipDistance(float distance);
		Camera* SetFarClipDistance(float distance);
		Camera* SetRenderTexture(RenderTexture* rt);

		std::string GetName();
		bool IsOrthographic();
		RGBA32 GetBackgroundColor();
		CameraClearFlag GetClearFlag();
		float GetAspectRatio();
		float GetFov();
		float GetSize();
		float GetNearClipDistance();
		float GetFarClipDistance();
		XMMATRIX GetViewMatrix();
		XMMATRIX GetProjectMatrix();
		RenderTexture* GetRenderTexture();
		Frustum GetViewSpaceFrustum();
		Frustum GetWorldSpaceFrustum();
	private:
		bool m_isEnabled = true;
		std::string m_name;
		Transform m_tranform;
		RGBA32 m_bgColor;
		CameraClearFlag m_clearFlag = CAMERA_CLEAR_SOLID_COLOR;
		float m_fov {0};
		float m_size {1};
		float m_nearClipDistance {0};
		float m_farClipDistance {1000};
		float m_aspectRatio = 0;
		bool m_isOrtho = false;
		RenderTexture* m_renderTexture;
	};
}