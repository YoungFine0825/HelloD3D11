#pragma once
#include "../Scene/Entity.h"
#include "../Scene/Camera.h"

namespace Framework 
{
	class FlyingCameraEntity : public Entity 
	{
	public:
		FlyingCameraEntity();
		FlyingCameraEntity(const std::string& name);
		~FlyingCameraEntity();
		//
		FlyingCameraEntity* AttachCamera(Camera* cam);
		FlyingCameraEntity* SetWalkSpeed(float walkSpeed);
		FlyingCameraEntity* SetSprintSpeed(float sprintSpeed);
		FlyingCameraEntity* SetTurningSpeed(float turningSpeed);
		//
		float GetCurSpeed() { return m_curSpeed; };
	protected:
		void OnTick(float dt) override;
	private:
		float m_speed{ 80.0f };
		float m_maxSpeed{ 150.0f };
		float m_curSpeed{ 0.0f };
		float m_turnSpeed{ 0.5f };
		Camera* m_camera;
	};
}