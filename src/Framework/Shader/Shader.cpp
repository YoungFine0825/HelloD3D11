
#include "../../Global.h"
#include "../../d3d/d3dGraphic.h"
#include "Shader.h"
#include "../Texture/Texture.h"

namespace Framework 
{
	Shader::Shader() :IResource() 
	{
		m_enabledTechName = "Default";
	}

	Shader::~Shader() 
	{
		IResource::~IResource();
	}

	void Shader::Release() 
	{
		m_pTech = nullptr;
		ReleaseCOM(m_pInputLayout);
		ReleaseCOM(m_pEffect);
	}

	bool Shader::CreateFromBlob(ID3D10Blob* compiledShader) 
	{
		ID3DX11Effect* newEffect;
		HRESULT hr = D3DX11CreateEffectFromMemory(
			compiledShader->GetBufferPointer(),
			compiledShader->GetBufferSize(),
			0,
			d3dGraphic::GetDevice(),
			&newEffect
		);
		if (FAILED(hr))
		{
			return false;
		}
		//
		ID3DX11EffectTechnique* newTech;
		newTech = newEffect->GetTechniqueByName(m_enabledTechName.c_str());
		if (newTech == nullptr)
		{
			ReleaseCOM(newEffect);
			return false;
		}
		//
		ID3DX11EffectPass* pass = newTech->GetPassByIndex(0);
		if (pass == nullptr) 
		{
			ReleaseCOM(newEffect);
			return false;
		}
		//
		ID3D11InputLayout* newLayout;
		if(!d3dGraphic::CreateInputLayout(pass, D3D_INPUT_DESC_COMMON, &newLayout))
		{
			ReleaseCOM(newEffect);
			return false;
		}
		//
		m_pTech = nullptr;
		ReleaseCOM(m_pInputLayout);
		ReleaseCOM(m_pEffect);
		//
		m_pEffect = newEffect;
		m_pTech = newTech;
		m_pInputLayout = newLayout;
		//
		return true;
	}

	bool Shader::SetEnabledTechnique(std::string techName)
	{
		if (m_enabledTechName == techName) 
		{
			return true;
		}
		if (m_pEffect == nullptr) 
		{
			return false;
		}
		ID3DX11EffectTechnique* tech = m_pEffect->GetTechniqueByName(techName.c_str());
		if (tech == nullptr) 
		{
			return false;
		}
		//
		ID3DX11EffectPass* pass = tech->GetPassByIndex(0);
		//
		if (pass == nullptr)
		{
			return false;
		}
		//
		ID3D11InputLayout* newLayout;
		if (!d3dGraphic::CreateInputLayout(pass, D3D_INPUT_DESC_COMMON, &newLayout))
		{
			return false;
		}
		//
		ReleaseCOM(m_pInputLayout);
		//
		m_pInputLayout = newLayout;
		m_pTech = tech;
		m_enabledTechName = techName;
		//
		return true;
	}

	UINT Shader::GetPassCount() 
	{
		if (m_pTech == nullptr) 
		{
			return 0;
		}
		D3DX11_TECHNIQUE_DESC techDesc;
		m_pTech->GetDesc(&techDesc);
		return techDesc.Passes;
	}

	bool Shader::ApplyPass(UINT passIndex, ID3D11DeviceContext* context)
	{
		if (m_pTech != nullptr) 
		{
			ID3DX11EffectPass* pass = m_pTech->GetPassByIndex(passIndex);
			if (pass) 
			{
				pass->Apply(0, context);
				return true;
			}
		}
		return false;
	}


	Shader* Shader::SetMatrix4x4(const char* propName, XMMATRIX matrix) 
	{
		if (m_pEffect == nullptr) 
		{
			return this;
		}
		ID3DX11EffectVariable* var = m_pEffect->GetVariableByName(propName);
		if (!var) { return this; }
		//
		ID3DX11EffectMatrixVariable* mat = var->AsMatrix();
		mat->SetMatrix(reinterpret_cast<float*>(&matrix));
		return this;
	}

	Shader* Shader::SetFloat(const char* propName, float value) 
	{
		if (m_pEffect == nullptr)
		{
			return this;
		}
		//
		ID3DX11EffectVariable* var = m_pEffect->GetVariableByName(propName);
		if (!var) { return this; }
		//
		ID3DX11EffectScalarVariable* scalar = var->AsScalar();
		scalar->SetFloat(value);
		return this;
	}

	Shader* Shader::SetVector2(const char* propName, XMFLOAT2 vec2)
	{
		if (m_pEffect == nullptr)
		{
			return this;
		}
		//
		ID3DX11EffectVariable* var = m_pEffect->GetVariableByName(propName);
		if (!var) { return this; }
		//
		ID3DX11EffectVectorVariable* vector = var->AsVector();
		vector->SetFloatVector((const float*)&vec2);
		return this;
	}

	Shader* Shader::SetVector3(const char* propName, XMFLOAT3 vec3)
	{
		if (m_pEffect == nullptr)
		{
			return this;
		}
		//
		ID3DX11EffectVariable* var = m_pEffect->GetVariableByName(propName);
		if (!var) { return this; }
		//
		ID3DX11EffectVectorVariable* vector = var->AsVector();
		vector->SetFloatVector((const float*)&vec3);
		return this;
	}

	Shader* Shader::SetVector4(const char* propName, XMFLOAT4 vec4)
	{
		if (m_pEffect == nullptr)
		{
			return this;
		}
		//
		ID3DX11EffectVariable* var = m_pEffect->GetVariableByName(propName);
		if (!var) { return this; }
		//
		ID3DX11EffectVectorVariable* vector = var->AsVector();
		vector->SetFloatVector((const float*)&vec4);
		return this;
	}

	Shader* Shader::SetStruct(const char* propName, void* pData, UINT Count, UINT Offset) 
	{
		if (m_pEffect == nullptr)
		{
			return this;
		}
		//
		ID3DX11EffectVariable* var = m_pEffect->GetVariableByName(propName);
		if (!var) { return this; }
		//
		var->SetRawValue(pData, Offset, Count);
		return this;
	}

	Shader* Shader::SetShaderResourceView(const char* propName, ID3D11ShaderResourceView* srv) 
	{
		if (m_pEffect == nullptr)
		{
			return this;
		}
		//
		ID3DX11EffectVariable* var = m_pEffect->GetVariableByName(propName);
		if (!var) { return this; }
		//
		ID3DX11EffectShaderResourceVariable* shaderResourceView = var->AsShaderResource();
		shaderResourceView->SetResource(srv);
		return this;
	}

	Shader* Shader::SetTexture(const char* propName, Texture* tex)
	{
		if (m_pEffect == nullptr)
		{
			return this;
		}
		//
		ID3DX11EffectVariable* var = m_pEffect->GetVariableByName(propName);
		if (!var) { return this; }
		//
		ID3DX11EffectShaderResourceVariable* shaderResourceView = var->AsShaderResource();
		ID3D11ShaderResourceView* srv = tex->GetShaderResourceView();
		shaderResourceView->SetResource(srv);
		return this;
	}

	Shader* Shader::SetUnorderredAccessView(const char* propName, ID3D11UnorderedAccessView* uav)
	{
		if (m_pEffect == nullptr)
		{
			return this;
		}
		//
		ID3DX11EffectVariable* var = m_pEffect->GetVariableByName(propName);
		if (!var) { return this; }
		//
		ID3DX11EffectUnorderedAccessViewVariable* unorderedAccessView = var->AsUnorderedAccessView();
		unorderedAccessView->SetUnorderedAccessView(uav);
		return this;
	}
}