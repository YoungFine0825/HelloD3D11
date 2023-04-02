#pragma once

#include "../Resource.h"
#include "../../math/MathLib.h"

struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3D11InputLayout;

namespace Framework 
{
	class Texture;

	class Shader : public Resource::IResource 
	{
	public:
		Shader();
		~Shader();
		void Release() override;
		//
		bool CreateFromBlob(ID3D10Blob* compiledShader);
		//
		bool SetEnabledTechnique(std::string techName);
		std::string GetEnabledTechniqueName() { return m_enabledTechName; }
		//
		UINT GetPassCount();
		bool ApplyPass(UINT passIndex, ID3D11DeviceContext* context);
		//
		ID3D11InputLayout* GetInputLayout() { return m_pInputLayout; }
		//
		Shader* SetMatrix4x4(const char* propName, XMMATRIX matrix);
		Shader* SetFloat(const char* propName, float value);
		Shader* SetVector2(const char* propName, XMFLOAT2 vec2);
		Shader* SetVector3(const char* propName, XMFLOAT3 vec3);
		Shader* SetVector4(const char* propName, XMFLOAT4 vec4);
		Shader* SetStruct(const char* propName, void* pData,UINT Count,UINT Offset = 0);
		Shader* SetShaderResourceView(const char* propName, ID3D11ShaderResourceView* srv);
		Shader* SetTexture(const char* propName, Texture* tex);
		Shader* SetUnorderredAccessView(const char* propName, ID3D11UnorderedAccessView* uav);
	private:
		ID3DX11Effect* m_pEffect;
		ID3DX11EffectTechnique* m_pTech;
		ID3D11InputLayout* m_pInputLayout;
		//
		std::string m_enabledTechName;
	};
}