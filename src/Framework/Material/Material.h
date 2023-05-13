#pragma once

#include "../../math/MathLib.h"
#include "../../Color.h"
#include "../Texture/Texture.h"
#include "../Shader/Shader.h"

#include <unordered_map>

namespace Framework 
{
	struct SimpleMaterial
	{
		RGBA32 ambientColor = { 1.0f,1.0f,1.0f,1.0f };
		RGBA32 diffuseColor = { 1.0f,1.0f,1.0f,1.0f };
		RGBA32 specularColor = { 1.0f,1.0f,1.0f,150.0f };
		XMFLOAT4 reflectColor = { 1.0f,1.0f,1.0f,1.0f };
		XMFLOAT4 diffuseMapST = { 1.0f,1.0f,0.0f,0.0f };
	};

	enum MaterialRenderQueue
	{
		RENDER_QUEUE_BACKGROUND = 0,
		RENDER_QUEUE_OPAQUE = 1,
		RENDER_QUEUE_ALPHA_TEST = 2,
		RENDER_QUEUE_TRANSPARENT = 3,
	};

	class Material 
	{
	public:
		Material();
		Material(std::string shaderPath);
		Material(Shader* shader);
		Material(const Material& mat);
		~Material();

		Material& operator=(const Material&);

		RGBA32 ambientColor = { 1.0f,1.0f,1.0f,1.0f };
		RGBA32 diffuseColor = { 1.0f,1.0f,1.0f,1.0f };
		RGBA32 specularColor = { 1.0f,1.0f,1.0f,1.0f };
		float specularPower = 100.0f;
		XMFLOAT4 reflectColor = { 1.0f,1.0f,1.0f,1.0f };
		

		Material* SetMainTexture(Texture* mainTex);
		Texture* GetMainTexture();
		XMFLOAT4 mainTextureST = { 1.0f,1.0f,0.0f,0.0f };

		Material* SetShader(Shader* sh);
		Shader* GetShader();


		Material* SetRenderQueue(MaterialRenderQueue queue);
		MaterialRenderQueue GetRenderQueue();

		Material* SetFloat(std::string key, float value);
		Material* SetFloat2(std::string key, XMFLOAT2 value);
		Material* SetFloat3(std::string key, XMFLOAT3 value);
		Material* SetFloat4(std::string key, XMFLOAT4 value);
		Material* SetMatrix(std::string key, XMMATRIX value);
		Material* SetTexture(std::string key, Texture* value);
		Material* Apply();

		Material* EnableCastShadow(bool enable);
		bool IsCastShadow();
		Material* EnableReceiveShadow(bool enable);
		bool IsReceiveShadow();
	private:

		Shader* m_shader;
		Texture* m_mainTexture;
		MaterialRenderQueue m_renderQueue = RENDER_QUEUE_OPAQUE;

		typedef std::unordered_map<std::string, float> FloatMap;
		FloatMap m_paramFloat;

		typedef std::unordered_map<std::string, XMFLOAT2> Float2Map;
		Float2Map m_paramFloat2;

		typedef std::unordered_map<std::string, XMFLOAT3> Float3Map;
		Float3Map m_paramFloat3;

		typedef std::unordered_map<std::string, XMFLOAT4> Float4Map;
		Float4Map m_paramFloat4;

		typedef std::unordered_map<std::string, XMMATRIX> MatrixMap;
		MatrixMap m_paramMat4x4;

		typedef std::unordered_map<std::string, Texture*> TextureMap;
		TextureMap m_paramTex;

		bool m_castShadow{ false };
		bool m_receiveShadow{ false };
	};
}
