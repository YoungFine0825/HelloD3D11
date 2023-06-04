#include "Shader.h"
#include "../../Global.h"
#include "../../d3d/d3dGraphic.h"
#include "../Texture/Texture.h"
#include "../RenderTexture/RenderTexture.h"

namespace Framework 
{
	Shader::Shader() :IResource() 
	{
		m_enabledTechName = "None";
	}

	Shader::~Shader() 
	{
		IResource::~IResource();
	}

	void Shader::Release() 
	{
		m_pTech = nullptr;
		for (UINT p = 0; p < m_passCount; ++p)
		{
			ReleaseCOM(m_pInputLayouts[p]);
		}
		ReleaseArrayPointer(m_pInputLayouts);
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
		m_pEffect = newEffect;
		//
		return SetEnabledTechnique("Default");
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
		D3DX11_TECHNIQUE_DESC techDesc;
		ZeroMemory(&techDesc, sizeof(techDesc));
		tech->GetDesc(&techDesc);
		UINT passCount = techDesc.Passes;
		if (passCount <= 0) 
		{
			return false;
		}
		//
		for (UINT p = 0; p < m_passCount; ++p) 
		{
			ReleaseCOM(m_pInputLayouts[p]);
		}
		ReleaseArrayPointer(m_pInputLayouts);
		m_pInputLayouts = new ID3D11InputLayout*[passCount];
		for (UINT p = 0; p < passCount; ++p) 
		{
			ID3DX11EffectPass* pass = tech->GetPassByIndex(p);
			if (pass != nullptr)
			{
				if (d3dGraphic::CreateInputLayout(pass, D3D_INPUT_DESC_COMMON, &m_pInputLayouts[p]))
				{
				}
			}
		}
		//
		m_pTech = tech;
		m_enabledTechName = techName;
		m_passCount = passCount;
		//
		return true;
	}

	bool Shader::hasTechnique(const std::string& techName) 
	{
		if (m_pEffect == nullptr)
		{
			return false;
		}
		ID3DX11EffectTechnique* tech = m_pEffect->GetTechniqueByName(techName.c_str());
		if (tech == nullptr)
		{
			return false;
		}
		D3DX11_TECHNIQUE_DESC techDesc;
		ZeroMemory(&techDesc, sizeof(techDesc));
		tech->GetDesc(&techDesc);
		if (techDesc.Passes <= 0) 
		{
			return false;
		}
		return true;
	}

	UINT Shader::GetPassCount() 
	{
		if (m_pTech == nullptr) 
		{
			return 0;
		}
		return m_passCount;
	}

	ID3D11InputLayout* Shader::GetInputLayout(UINT passIndex)
	{
		if (passIndex >= m_passCount) 
		{
			return nullptr;
		}
		return m_pInputLayouts[passIndex];
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

	bool Shader::hasPass(const std::string passName)
	{
		if (!m_pTech) 
		{
			return false;
		}
		ID3DX11EffectPass* pass = m_pTech->GetPassByName(passName.c_str());
		if (!pass || !pass->IsValid()) 
		{
			return false;
		}
		return true;
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

	Shader* Shader::SetArray(const char* propName, void* pData, UINT Count, UINT Offset)
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
		if (tex) 
		{
			ID3D11ShaderResourceView* srv = tex->GetShaderResourceView();
			shaderResourceView->SetResource(srv);
		}
		else 
		{
			shaderResourceView->SetResource(nullptr);
		}
		return this;
	}

	Shader* Shader::SetRenderTexture(const char* propName, RenderTexture* rt)
	{
		if (rt == nullptr)
		{
			return this;
		}
		if (m_pEffect == nullptr)
		{
			return this;
		}
		SetShaderResourceView(propName, rt->GetColorTextureSRV());
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

	Shader* Shader::SetInputLayout(unsigned int layoutIndex)
	{
		if (!m_pTech) 
		{
			return this;
		}
		//
		if (m_passCount <= 0)
		{
			return this;
		}
		//
		for (UINT p = 0; p < m_passCount; ++p)
		{
			ReleaseCOM(m_pInputLayouts[p]);
		}
		ReleaseArrayPointer(m_pInputLayouts);
		m_pInputLayouts = new ID3D11InputLayout * [m_passCount];
		for (UINT p = 0; p < m_passCount; ++p)
		{
			ID3DX11EffectPass* pass = m_pTech->GetPassByIndex(p);
			if (pass != nullptr)
			{
				if (d3dGraphic::CreateInputLayout(pass, layoutIndex, &m_pInputLayouts[p]))
				{
				}
			}
		}
		return this;
	}
}