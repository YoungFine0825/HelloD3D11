#pragma once

#include "DInput.h"
#include "windows.h"

bool dxinput_Init(HINSTANCE hInstance, HWND hWnd);
void dxinput_Tick();
void dxinput_Cleanup();
bool dxinput_IsKeyDown(int DIK);
bool dxinput_IsMousePress(int btnId);
float dxinput_GetMousePosDeltaX();
float dxinput_GetMousePosDeltaY();
float dxinput_GetMousePosDeltaZ();