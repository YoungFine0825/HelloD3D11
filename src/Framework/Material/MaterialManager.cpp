#include <unordered_map>
#include <istream>
#include <fstream>
#include <vector>
#include "MaterialManager.h"
#include "../Texture/TextureManager.h"
#include "../Shader/ShaderManager.h"
#include "../../Global.h"

#define MATERIAL_PROPERTY_TYPE_COLOR "Color"
#define MATERIAL_PROPERTY_TYPE_TEXTURE "Texture"
#define MATERIAL_PROPERTY_TYPE_VECTOR "Vector"
#define MATERIAL_PROPERTY_TYPE_FLOAT "Float"

#define MATERIAL_PROP_MAIN_TEXTIRE "mainTexture"
#define MATERIAL_PROP_MAIN_TEXTIRE_ST "mainTextureST"
#define MATERIAL_PROP_AMIEBNT_COLOR "ambientColor"
#define MATERIAL_PROP_DIFFUSE_COLOR "diffuseColor"
#define MATERIAL_PROP_SPEACULAR_COLOR "specularColor"
#define MATERIAL_PROP_REFLECT_COLOR "reflectColor"
#define MATERIAL_PROP_SPECULAR_POWER "specularPower"

#define MATERIAL_TAGS_RENDER_TYPE "RenderType"
#define MATERIAL_TAGS_CAST_SHADOW "CastShadow"
#define MATERIAL_TAGS_RECEIVE_SHADOW "ReceiveShadow"

#define MATERIAL_RENDER_TYPE_BACKGROUND "Background"
#define MATERIAL_RENDER_TYPE_OPAQUE "Background"
#define MATERIAL_RENDER_TYPE_ALPHA_TEST "Background"
#define MATERIAL_RENDER_TYPE_TRANSPARENT "Transparent"

namespace Framework 
{
	namespace MaterialManager 
	{
		typedef std::unordered_map<Resource::ResourceUrl, Material*> MaterialResMap;
		MaterialResMap m_materialsResMap;

		std::string token;

		Material* FindMaterial(Resource::ResourceUrl url) 
		{
			MaterialResMap::iterator it = m_materialsResMap.find(url);
			if (it == m_materialsResMap.end())
			{
				return nullptr;
			}
			return it->second;
		}

		void GrabMaterialPropertyValue(std::ifstream& fs, std::string& type, std::string& name, Material* material)
		{
			if (type == MATERIAL_PROPERTY_TYPE_TEXTURE) 
			{
				fs >> token;
				std::string texUrl = ASSETS_BASE_PATH + token;
				Texture* tex = TextureManager::LoadFromFile(texUrl);
				if (name == MATERIAL_PROP_MAIN_TEXTIRE) 
				{
					material->SetMainTexture(tex);
				}
				else 
				{
					material->SetTexture(name, tex);
				}
			}
			else if (type == MATERIAL_PROPERTY_TYPE_COLOR || type == MATERIAL_PROPERTY_TYPE_VECTOR)
			{
				fs >> token;
				std::string firstValue = token.substr(1, token.length() - 1);
				std::vector<float> values = {std::stof(firstValue)};
				while(fs >> token) 
				{
					if (token[token.length() - 1] == ')') 
					{
						values.push_back(std::stof(token.substr(0, token.length() - 1)));
						break;
					}
					else 
					{
						values.push_back(std::stof(token));
					}
				}
				size_t numValues = values.size();
				if (numValues == 2) 
				{
					material->SetFloat2(name, { values[0],values[1] });
				}
				else if (numValues == 3) 
				{
					material->SetFloat3(name, { values[0],values[1],values[2] });
				}
				else if (numValues == 4) 
				{
					if (name == MATERIAL_PROP_AMIEBNT_COLOR) 
					{
						material->ambientColor = { values[0],values[1],values[2] ,values[3] };
					}
					else if (name == MATERIAL_PROP_DIFFUSE_COLOR) 
					{
						material->ambientColor = { values[0],values[1],values[2] ,values[3] };
					}
					else if (name == MATERIAL_PROP_SPEACULAR_COLOR)
					{
						material->specularColor = { values[0],values[1],values[2] ,values[3] };
					}
					else if (name == MATERIAL_PROP_REFLECT_COLOR)
					{
						material->reflectColor = { values[0],values[1],values[2] ,values[3] };
					}
					else if (name == MATERIAL_PROP_MAIN_TEXTIRE_ST)
					{
						material->mainTextureST = { values[0],values[1],values[2] ,values[3] };
					}
					else 
					{
						material->SetFloat4(name, { values[0],values[1],values[2] ,values[3] });
					}
				}
			}
			else if (type == MATERIAL_PROPERTY_TYPE_FLOAT) 
			{
				fs >> token;
				if (name == MATERIAL_PROP_SPECULAR_POWER) 
				{
					material->specularPower = std::stof(token);
				}
				else 
				{
					material->SetFloat(name,std::stof(token));
				}
			}
		}

		void ParseMaterialProperties(std::ifstream& fs, Material* material) 
		{
			while (fs >> token) 
			{
				if (token == "}")
				{
					break;
				}
				std::string type = token;
				fs >> token;
				std::string name = token;
				GrabMaterialPropertyValue(fs, type, name, material);
			}
		}

		void ParseMaterialTags(std::ifstream& fs, Material* material)
		{
			while (fs >> token) 
			{
				if (token == "}")
				{
					break;
				}
				if (token == MATERIAL_TAGS_RENDER_TYPE) 
				{
					fs >> token;
					if (token == MATERIAL_RENDER_TYPE_BACKGROUND) 
					{
						material->SetRenderQueue(RENDER_QUEUE_BACKGROUND);
					}
					else if (token == MATERIAL_RENDER_TYPE_OPAQUE)
					{
						material->SetRenderQueue(RENDER_QUEUE_OPAQUE);
					}
					else if (token == MATERIAL_RENDER_TYPE_ALPHA_TEST)
					{
						material->SetRenderQueue(RENDER_QUEUE_ALPHA_TEST);
					}
					else if (token == MATERIAL_RENDER_TYPE_TRANSPARENT)
					{
						material->SetRenderQueue(RENDER_QUEUE_TRANSPARENT);
					}
				}
				else if (token == MATERIAL_TAGS_CAST_SHADOW) 
				{
					fs >> token;
					material->EnableCastShadow(std::stoi(token) == 1);
				}
				else if (token == MATERIAL_TAGS_RECEIVE_SHADOW)
				{
					fs >> token;
					material->EnableReceiveShadow(std::stoi(token) == 1);
				}
			}
		}

		Material* LoadMaterialFromFile(Resource::ResourceUrl url)
		{
			std::ifstream file(url);
			if(!file)
			{
				return nullptr;
			}
			file >> token;//Name
			file >> token;
			std::string name = token;
			file >> token;//Shader
			file >> token;
			std::string shaderUrl = ASSETS_BASE_PATH +token;
			file >> token;//Properties
			file >> token;//{
			Material* newMaterial = new Material();
			ParseMaterialProperties(file, newMaterial);
			//
			file >> token;//Tags
			file >> token;//{
			ParseMaterialTags(file, newMaterial);
			//
			Shader* shader = ShaderManager::FindWithUrl(shaderUrl);
			if (!shader)
			{
				shader = ShaderManager::LoadFromFxFile(shaderUrl);
			}
			newMaterial->SetShader(shader);
			//
			return newMaterial;
		}

		Material* CreateMaterialInstance(Resource::ResourceUrl url) 
		{
			Material* mat = FindMaterial(url);
			if (mat) 
			{
				Material* matInstance = new Material();
				(*matInstance) = (*mat);
				return matInstance;
			}
			//
			Material* newMaterial = LoadMaterialFromFile(url);
			if (!newMaterial) 
			{
				return nullptr;
			}
			m_materialsResMap[url] = newMaterial;
			Material* materialInst = new Material();
			(*materialInst) = (*newMaterial);
			return materialInst;
		}

		void Cleanup() 
		{
			MaterialResMap::iterator it;
			for (it = m_materialsResMap.begin(); it != m_materialsResMap.end(); ++it)
			{
				delete it->second;
			}
			m_materialsResMap.clear();
		}
	}
}