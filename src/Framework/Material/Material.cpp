
#include "Material.h"

#include "../Shader/ShaderManager.h"

namespace Framework
{
	Material::Material()
	{

	}

	Material::Material(std::string shaderPath)
	{
		Shader* shader = ShaderManager::FindWithUrl(shaderPath);
		if (!shader) 
		{
			shader = ShaderManager::LoadFromFxFile(shaderPath);
		}
		m_shader = shader;
	}

	Material::Material(Shader* shader) 
	{
		m_shader = shader;
	}

	Material::Material(const Material& mat)
	{
		ambientColor = mat.ambientColor;
		diffuseColor = mat.diffuseColor;
		specularColor = mat.specularColor;
		reflectColor = mat.reflectColor;
		//
		m_shader = mat.m_shader;
		m_mainTexture = mat.m_mainTexture;
		mainTextureST = mat.mainTextureST;
		//
		m_paramFloat = mat.m_paramFloat;
		m_paramFloat2 = mat.m_paramFloat2;
		m_paramFloat3 = mat.m_paramFloat3;
		m_paramFloat4 = mat.m_paramFloat4;
		m_paramMat4x4 = mat.m_paramMat4x4;
		m_paramTex = mat.m_paramTex;
	}

	Material::~Material()
	{
		m_shader = nullptr;
		m_mainTexture = nullptr;
		//
		m_paramFloat.clear();
		m_paramFloat2.clear();
		m_paramFloat3.clear();
		m_paramFloat4.clear();
		m_paramMat4x4.clear();
		m_paramTex.clear();
	}

	Material& Material::operator=(const Material& mat) 
	{
		ambientColor = mat.ambientColor;
		diffuseColor = mat.diffuseColor;
		specularColor = mat.specularColor;
		reflectColor = mat.reflectColor;
		specularPower = mat.specularPower;
		//
		m_shader = mat.m_shader;
		m_mainTexture = mat.m_mainTexture;
		mainTextureST = mat.mainTextureST;
		m_castShadow = mat.m_castShadow;
		m_receiveShadow = mat.m_receiveShadow;
		m_renderQueue = mat.m_renderQueue;
		//
		m_paramFloat = mat.m_paramFloat;
		m_paramFloat2 = mat.m_paramFloat2;
		m_paramFloat3 = mat.m_paramFloat3;
		m_paramFloat4 = mat.m_paramFloat4;
		m_paramMat4x4 = mat.m_paramMat4x4;
		m_paramTex = mat.m_paramTex;
		return *this;
	}

	Material* Material::SetMainTexture(Texture* mainTex) 
	{
		m_mainTexture = mainTex;
		return this;
	}

	Texture* Material::GetMainTexture() 
	{
		return m_mainTexture;
	}

	Material* Material::SetShader(Shader* sh) 
	{
		m_shader = sh;
		return this;
	}

	Shader* Material::GetShader() 
	{
		return m_shader;
	}

	Material* Material::SetFloat(std::string key, float value) 
	{
		m_paramFloat[key] = value;
		return this;
	}

	Material* Material::SetFloat2(std::string key, XMFLOAT2 value)
	{
		m_paramFloat2[key] = value;
		return this;
	}

	Material* Material::SetFloat3(std::string key, XMFLOAT3 value)
	{
		m_paramFloat3[key] = value;
		return this;
	}

	Material* Material::SetFloat4(std::string key, XMFLOAT4 value)
	{
		m_paramFloat4[key] = value;
		return this;
	}

	Material* Material::SetMatrix(std::string key, XMMATRIX value)
	{
		m_paramMat4x4[key] = value;
		return this;
	}

	Material* Material::SetTexture(std::string key, Texture* value)
	{
		m_paramTex[key] = value;
		return this;
	}

	Material* Material::Apply() 
	{
		if (m_shader == nullptr) 
		{
			return this;
		}
		//
		SimpleMaterial matParam;
		matParam.diffuseColor = diffuseColor;
		matParam.ambientColor = ambientColor;
		matParam.specularColor = specularColor;
		matParam.specularColor.w = specularPower;
		matParam.reflectColor = reflectColor;
		matParam.diffuseMapST = mainTextureST;
		m_shader->SetStruct("obj_Material", &matParam, sizeof(matParam));
		//
		if (m_mainTexture != nullptr) 
		{
			m_shader->SetTexture("g_diffuseMap", m_mainTexture);
		}
		else 
		{
			m_shader->SetTexture("g_diffuseMap", nullptr);
		}
		//
		for (FloatMap::iterator it = m_paramFloat.begin(); it != m_paramFloat.end(); ++it) 
		{
			m_shader->SetFloat(it->first.c_str(), it->second);
		}
		//
		for (Float2Map::iterator it = m_paramFloat2.begin(); it != m_paramFloat2.end(); ++it) 
		{
			m_shader->SetVector2(it->first.c_str(), it->second);
		}
		//
		for (Float3Map::iterator it = m_paramFloat3.begin(); it != m_paramFloat3.end(); ++it)
		{
			m_shader->SetVector3(it->first.c_str(), it->second);
		}
		//
		for (Float4Map::iterator it = m_paramFloat4.begin(); it != m_paramFloat4.end(); ++it)
		{
			m_shader->SetVector4(it->first.c_str(), it->second);
		}
		//
		for (MatrixMap::iterator it = m_paramMat4x4.begin(); it != m_paramMat4x4.end(); ++it)
		{
			m_shader->SetMatrix4x4(it->first.c_str(), it->second);
		}
		//
		for (TextureMap::iterator it = m_paramTex.begin(); it != m_paramTex.end(); ++it)
		{
			m_shader->SetTexture(it->first.c_str(), it->second);
		}
		//
		return this;
	}

	Material* Material::EnableLighting(bool enable) 
	{
		m_enableLighting = enable;
		return this;
	}

	bool Material::IsEnableLighting() 
	{
		return m_enableLighting;
	}

	Material* Material::EnableCastShadow(bool enable) 
	{
		m_castShadow = enable;
		return this;
	}

	bool Material::IsCastShadow() 
	{
		return m_castShadow;
	}

	Material* Material::EnableReceiveShadow(bool enable) 
	{
		m_receiveShadow = enable;
		return this;
	}

	bool Material::IsReceiveShadow() 
	{
		return m_receiveShadow;
	}

	Material* Material::SetRenderQueue(MaterialRenderQueue queue) 
	{
		m_renderQueue = queue;
		return this;
	}

	MaterialRenderQueue Material::GetRenderQueue() 
	{
		return m_renderQueue;
	}
}