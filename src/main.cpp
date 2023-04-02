

#include "Window.h"
#include "App.h"

#include <string>

#include "../../imgui-1.87/backends/imgui_impl_win32.h"
// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int m_winWid = 800;
int m_winHei = 600;

std::wstring m_winTitle = L"Hello D3D11";

HWND m_hWnd;
HINSTANCE m_hInstance;
DWORD m_winStyle = WS_OVERLAPPEDWINDOW;

int m_timeNow = 0;
int m_timePre = 0;

#define WND_CLASS_NAME L"HelloD3D11"

LRESULT CALLBACK WndProc(HWND hWNd, UINT message, WPARAM wParam, LPARAM lParam) 
{

	if (ImGui_ImplWin32_WndProcHandler(hWNd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hWNd);
		}
		break;
	case WM_DESTROY:
		//
		App_Cleanup();
		//
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		ValidateRect(hWNd, nullptr);
		break;
	default:
		return DefWindowProc(hWNd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) 
{
	m_hInstance = hInstance;
	//
	WNDCLASSEX wndClassEx = { 0 };
	wndClassEx.cbSize = sizeof(wndClassEx);
	wndClassEx.hInstance = hInstance;
	wndClassEx.lpszClassName = WND_CLASS_NAME;
	wndClassEx.lpfnWndProc = WndProc;
	wndClassEx.style = CS_VREDRAW | CS_HREDRAW;
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbWndExtra = 0;
	//
	if (!RegisterClassEx(&wndClassEx)) 
	{
		return -1;
	}
	//
	App_PreCreateWindow();
	//
	m_hWnd = CreateWindow(WND_CLASS_NAME, m_winTitle.c_str(), m_winStyle, 0, 0, m_winWid, m_winHei, nullptr, nullptr, hInstance, nullptr);
	//
	MoveWindow(m_hWnd, 0, 0, m_winWid, m_winHei, true);
	UpdateWindow(m_hWnd);
	ShowWindow(m_hWnd, nShowCmd);
	//
	if (!App_Init()) 
	{
		win_MsgBox(L"初始化应用失败！");
		App_Cleanup();
		return -1;
	}
	//
	m_timePre = ::GetTickCount();
	//
	MSG msg = { 0 };
	while (msg.message != WM_QUIT) 
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
			m_timeNow = GetTickCount();
			int timeDelta = m_timeNow - m_timePre;
			m_timePre = m_timeNow;
			//
			App_Tick((float)timeDelta / 1000.0f);
			//
			if (timeDelta >= 16) 
			{
				App_Draw();
			}
		}
	}
	//
	UnregisterClass(WND_CLASS_NAME, hInstance);
	//
	return 0;
}

HWND win_GetHandle() 
{
	return m_hWnd;
}

HINSTANCE win_GetInstance() 
{
	return m_hInstance;
}

void win_SetWidth(int width) 
{
	m_winWid = width;
}

void win_SetHeight(int height) 
{
	m_winHei = height;
}

void win_SetTitle(LPCWSTR title)
{
	m_winTitle = title;
}

int win_GetWidth() 
{ 
	return m_winWid;
}

int win_GetHeight() 
{
	return m_winHei;
}

void win_MsgBox(LPCWSTR content) 
{
	MessageBox(0, content, L"提示", 0);
}

bool win_IsFullScreen() 
{
	return false;
}

float win_GetAspectRatio() 
{
	return (float)m_winWid / (float)m_winHei;
}