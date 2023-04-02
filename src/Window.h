#pragma once

#include<Windows.h>

HWND win_GetHandle();
HINSTANCE win_GetInstance();

int win_GetWidth();
int win_GetHeight();
float win_GetAspectRatio();
void win_SetWidth(int width);
void win_SetHeight(int height);
void win_SetTitle(LPCWSTR title);
void win_MsgBox(LPCWSTR content);
bool win_IsFullScreen();