
#include "d3dGraphic.h"
#include "d3dEffectsUtil.h"
#include <DxErr.h>

namespace d3dEffectsUtil 
{
	bool TryCompileShaderFromFxFile(const char* filePath, ID3D10Blob** shader, ID3D10Blob** errMsg) 
	{
		DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
		HRESULT hr = D3DX11CompileFromFileA(filePath, 0, 0, 0, "fx_5_0", shaderFlags,
			0, 0, shader, errMsg, 0);

		// compilationMsgs can store errors or warnings.
		if ((*errMsg) != 0)
		{
			MessageBoxA(0, (char*)(*errMsg)->GetBufferPointer(), 0, 0);
			(*errMsg)->Release();
			(*errMsg) = nullptr;
		}

		// Even if there are no compilationMsgs, check to make sure there were no other errors.
		if (FAILED(hr))
		{
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11CompileFromFile", true);
			return false;
		}
		return true;
	}

	bool CreateEffectFromFxFile(const char* filePath, ID3DX11Effect** pEffect) 
	{
		ID3D10Blob* compiledShader;
		ID3D10Blob* errMsg;
		if (!TryCompileShaderFromFxFile(filePath, &compiledShader, &errMsg)) 
		{
			return false;
		}
		//
		HRESULT hr = D3DX11CreateEffectFromMemory(
			compiledShader->GetBufferPointer(), 
			compiledShader->GetBufferSize(),
			0, 
			d3dGraphic::GetDevice(), 
			pEffect
		);
		if (FAILED(hr)) 
		{
			return false;
		}
		compiledShader->Release();
		compiledShader = nullptr;
		return true;
	}

	void DrawIndexed(ID3DX11Effect* effect, UINT IndexCount, UINT StartIndexLocation, int BaseVertexLocation) 
	{
		if (effect == nullptr) { return; }
		ID3D11DeviceContext* context = d3dGraphic::GetDeviceContext();
		if (context == nullptr) { return; }
		//
		ID3DX11EffectTechnique* tech = effect->GetTechniqueByIndex(0);
		//
		DrawIndexed(tech, IndexCount, StartIndexLocation, BaseVertexLocation);
	}

	void DrawIndexed(ID3DX11EffectTechnique* effTech, UINT IndexCount, UINT StartIndexLocation, int BaseVertexLocation)
	{
		if (effTech == nullptr) { return; }
		ID3D11DeviceContext* context = d3dGraphic::GetDeviceContext();
		if (context == nullptr) { return; }
		//
		D3DX11_TECHNIQUE_DESC techDesc;
		effTech->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			effTech->GetPassByIndex(p)->Apply(0, context);

			// 36 indices for the box.
			context->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
		}
	}


	bool SetMatrix4x4(ID3DX11Effect* pEffect, LPCSTR propName, const float* pData)
	{
		if (!pEffect) { return false; }
		//
		ID3DX11EffectVariable* var = pEffect->GetVariableByName(propName);
		if (!var) { return false; }
		//
		ID3DX11EffectMatrixVariable* mat = var->AsMatrix();
		mat->SetMatrix(pData);
		return true;
	}

	bool SetMatrix4x4(ID3DX11Effect* pEffect, LPCSTR propName, XMMATRIX matrix) 
	{
		if (!pEffect) { return false; }
		//
		ID3DX11EffectVariable* var = pEffect->GetVariableByName(propName);
		if (!var) { return false; }
		//
		ID3DX11EffectMatrixVariable* mat = var->AsMatrix();
		mat->SetMatrix(reinterpret_cast<float*>(&matrix));
		return true;
	}

	bool SetFloat(ID3DX11Effect* pEffect, LPCSTR propName, float value) 
	{
		if (!pEffect) { return false; }
		//
		ID3DX11EffectVariable* var = pEffect->GetVariableByName(propName);
		if (!var) { return false; }
		//
		ID3DX11EffectScalarVariable* scalar = var->AsScalar();
		scalar->SetFloat(value);
		return true;
	}
}