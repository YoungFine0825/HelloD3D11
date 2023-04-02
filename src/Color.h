#pragma once

#include "math/MathLib.h"

typedef XMFLOAT4 RGBA32;

namespace Colors
{
	const RGBA32 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	const RGBA32 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	const RGBA32 Gray = { 0.5f, 0.5f, 0.5f, 1.0f };
	const RGBA32 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	const RGBA32 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	const RGBA32 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	const RGBA32 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	const RGBA32 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	const RGBA32 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
	const RGBA32 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
	const RGBA32 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
	const RGBA32 SunLight = { 1.0f, 0.95f, 0.84f, 1.0f };
}