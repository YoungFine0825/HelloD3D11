#pragma once
#include "../../math/MathLib.h"
#include "../Collision/Collision.h"
#include "../Scene/Light.h"
#include "../Scene/Camera.h"
#include "Renderer.h"
#include <vector>

namespace Framework 
{
	struct SceneSettingData
	{
		bool enableFog = false;
		RGBA32 linearFogColor = { 0.5f,0.5f,0.5f,1.0f };
		float linearFogStart = 100;
		float linearFogRange = 2000;
	};

	class FrameData
	{
	public:
		FrameData();
		~FrameData();
		//
		std::vector<Camera*> cameras;
		RendererVector renderers;
		LightVector lights;
		//
		SceneSettingData sceneSetting;
		void Reset();
	};
}