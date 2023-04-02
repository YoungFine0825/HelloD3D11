#include "FlyingCameraEntity.h"
#include "../../d3d/DXInput.h"
#include "../../math/MathLib.h"

namespace Framework 
{
	FlyingCameraEntity::FlyingCameraEntity() :Entity() 
	{

	}

	FlyingCameraEntity::FlyingCameraEntity(const std::string& name) : Entity(name) 
	{

	}

	FlyingCameraEntity::~FlyingCameraEntity() 
	{
		m_camera = nullptr;
		Entity::~Entity();
	}

	FlyingCameraEntity* FlyingCameraEntity::AttachCamera(Camera* cam)
	{
		m_camera = cam;
		return this;
	}

	void FlyingCameraEntity::OnTick(float dt) 
	{
		if (!m_camera) { return; }
		//
		Transform* cameraTrans = m_camera->GetTransform();
		XMFLOAT3 positionW = cameraTrans->position;
		XMFLOAT3 rotationW = cameraTrans->rotation;
		//
		XMFLOAT3 movingSpeed = { 0,0,0 };
		if (dxinput_IsKeyDown(DIK_W)) 
		{
			movingSpeed.z = 1.0f;
		}
		if (dxinput_IsKeyDown(DIK_S)) 
		{
			movingSpeed.z = -1.0f;
		}
		if (dxinput_IsKeyDown(DIK_A))
		{
			movingSpeed.x = -1.0f;
		}
		if (dxinput_IsKeyDown(DIK_D))
		{
			movingSpeed.x = 1.0f;
		}
		m_curSpeed = dxinput_IsKeyDown(DIK_LSHIFT) ? m_maxSpeed : m_speed;
		movingSpeed.x *= m_curSpeed * dt;
		movingSpeed.z *= m_curSpeed * dt;
		//
		if (dxinput_IsMousePress(1))
		{
			float mouseX = dxinput_GetMousePosDeltaX();
			float mouseY = dxinput_GetMousePosDeltaY();
			//
			rotationW.x += mouseY * m_turnSpeed;
			rotationW.y += mouseX * m_turnSpeed;
			//
			if (rotationW.x > 80.0f) { rotationW.x = 80.0f; }
			if (rotationW.x < -80.0f) { rotationW.x = -80.0f; }
		}
		//
		XMMATRIX rotMatrix = XMMatrixRotationFromFloat3(rotationW);
		XMFLOAT3 forwardW = XMVectorNormalize(XMFloat3MultiMatrix({ 0,0,1 }, rotMatrix));
		XMFLOAT3 upW = XMVectorNormalize(XMFloat3MultiMatrix({ 0,1,0 }, rotMatrix));
		XMFLOAT3 rightW = XMVectorNormalize(XMVectorCross(upW, forwardW));
		XMFLOAT3 newPosW = positionW + forwardW * movingSpeed.z + rightW * movingSpeed.x;
		//
		cameraTrans->position = newPosW;
		cameraTrans->rotation = rotationW;
	}

	FlyingCameraEntity* FlyingCameraEntity::SetWalkSpeed(float walkSpeed) 
	{
		m_speed = walkSpeed;
		return this;
	}

	FlyingCameraEntity* FlyingCameraEntity::SetSprintSpeed(float sprintSpeed) 
	{
		m_maxSpeed = sprintSpeed;
		return this;
	}

	FlyingCameraEntity* FlyingCameraEntity::SetTurningSpeed(float turningSpeed) 
	{
		m_turnSpeed = turningSpeed;
		return this;
	}
}