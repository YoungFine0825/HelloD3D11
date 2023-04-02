
#include "ImGuiHelper.h"
#include "../../imgui-1.87/backends/imgui_impl_win32.h"
#include "../../imgui-1.87/backends/imgui_impl_dx11.h"
#include "../Graphic.h"
#include "../../Window.h"

namespace Framework 
{
	namespace ImGuiHelper 
	{
		void Init()
		{
			//³õÊ¼»¯Dear ImGui
			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

			// Setup Dear ImGui style
			//ImGui::StyleColorsDark();
			//ImGui::StyleColorsClassic();
			ImGui::StyleColorsLight();

			ImGui_ImplWin32_Init(win_GetHandle());
			ImGui_ImplDX11_Init(d3dGraphic::GetDevice(), d3dGraphic::GetDeviceContext());
			// Load Fonts
			ImFont* font = io.Fonts->AddFontFromFileTTF("res/fonts/font.ttf", 15.0f, NULL, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
			IM_ASSERT(font != NULL);
		}

		void BeginGUI() 
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		}

		void EndGUI() 
		{
			// Rendering
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		void Cleanup() 
		{
			// Cleanup
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
		}
	}
}