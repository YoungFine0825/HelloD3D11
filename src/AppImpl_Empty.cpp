
#include "App.h"

#ifdef AppImpl_Empty

#include "Window.h"

void App_PreCreateWindow()
{
	win_SetWidth(1280);
	win_SetTitle(L"AppImpl_Empty");
}

bool App_Init() 
{
	return true;
}

void App_Tick(float dt) 
{

}

void App_Draw() 
{

}

void App_Cleanup() 
{

}

#endif // AppImpl_Empty
