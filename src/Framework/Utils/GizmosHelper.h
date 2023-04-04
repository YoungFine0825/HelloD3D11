#pragma once


#include "../math/MathLib.h"
#include "../../Color.h"
#include "../Collision/xnacollision.h"

namespace Framework 
{
	namespace GizmosHelper
	{
		void Init();
		void Cleanup();
		void DrawCube(XMMATRIX mvp,RGBA32 color);
		void DrawAABB(XNA::AxisAlignedBox localAABB, XMMATRIX mvp, RGBA32 color);
		void DrawFrustum(XNA::Frustum frustum, XMMATRIX mvp, RGBA32 color);
	}
}