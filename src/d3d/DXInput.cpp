
#include "DXInput.h"

IDirectInput8* g_dInput8;

IDirectInputDevice8* m_keyboradDev;
char m_keyBuffer[256];

IDirectInputDevice8* m_mouseDev;
DIMOUSESTATE m_mouseState;

bool dxinput_Init(HINSTANCE hInstance, HWND hWnd) 
{
	HRESULT hr;
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_dInput8, nullptr))) 
	{
		return false;
	}
	//
	if (SUCCEEDED(g_dInput8->CreateDevice(GUID_SysKeyboard, &m_keyboradDev, nullptr))) 
	{
		m_keyboradDev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND);
		m_keyboradDev->SetDataFormat(&c_dfDIKeyboard);
		m_keyboradDev->Acquire();
		m_keyboradDev->Poll();
	}
	else 
	{
		return false;
	}
	//
	if (SUCCEEDED(g_dInput8->CreateDevice(GUID_SysMouse, &m_mouseDev, nullptr))) 
	{
		m_mouseDev->SetCooperativeLevel(hWnd, DISCL_FOREGROUND);
		m_mouseDev->SetDataFormat(&c_dfDIMouse);
		m_mouseDev->Acquire();
		m_mouseDev->Poll();
	}
	else 
	{
		if (m_keyboradDev) 
		{
			m_keyboradDev->Release();
			m_keyboradDev = nullptr;
		}
		return false;
	}
	return true;
}

void dxinput_Tick() 
{
	if (FAILED(m_keyboradDev->GetDeviceState(sizeof(m_keyBuffer), (void**)&m_keyBuffer))) 
	{
		m_keyboradDev->Acquire();
		m_keyboradDev->GetDeviceState(sizeof(m_keyBuffer), (LPVOID)m_keyBuffer);
	}
	//
	if (FAILED(m_mouseDev->GetDeviceState(sizeof(m_mouseState), (void**)&m_mouseState))) 
	{
		m_mouseDev->Acquire();
		m_mouseDev->GetDeviceState(sizeof(m_mouseState), (void**)&m_mouseState);
	}
}

void dxinput_Cleanup() 
{
	if (m_mouseDev != nullptr) 
	{
		m_mouseDev->Release();
		m_mouseDev = nullptr;
	}
	if (m_keyboradDev != nullptr) 
	{
		m_keyboradDev->Release();
		m_keyboradDev = nullptr;
	}
}

bool dxinput_IsKeyDown(int DIK) 
{
	if (m_keyBuffer[DIK] & 0x80) 
	{
		return true;
	}
	else 
	{
		return false;
	}
}

bool dxinput_IsMousePress(int btnId) 
{
	if (m_mouseState.rgbButtons[btnId] & 0x80) 
	{
		return true;
	}
	else 
	{
		return false;
	}
}

float dxinput_GetMousePosDeltaX() 
{
	return (float)m_mouseState.lX;
}

float dxinput_GetMousePosDeltaY()
{
	return (float)m_mouseState.lY;
}

float dxinput_GetMousePosDeltaZ()
{
	return (float)m_mouseState.lZ;
}