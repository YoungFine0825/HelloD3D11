#pragma once

#include <d3dx11effect.h>

#include "../math/MathLib.h"

namespace d3dEffectsUtil 
{
	bool CreateEffectFromFxFile(const char* filePath, ID3DX11Effect** pEffect);
	//
	void DrawIndexed(ID3DX11Effect* effect, UINT IndexCount, UINT StartIndexLocation = 0,int BaseVertexLocation = 0);
	void DrawIndexed(ID3DX11EffectTechnique* effTech, UINT IndexCount, UINT StartIndexLocation = 0, int BaseVertexLocation = 0);
	//
	bool SetMatrix4x4(ID3DX11Effect* pEffect, LPCSTR propName, const float* pData);
	bool SetMatrix4x4(ID3DX11Effect* pEffect, LPCSTR propName, XMMATRIX matrix);
	bool SetFloat(ID3DX11Effect* pEffect, LPCSTR propName, float value);
}