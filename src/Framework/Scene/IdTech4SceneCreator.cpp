#include "IdTech4SceneCreator.h"
#include "SceneManager.h"
#include "Entity.h"
#include "Camera.h"
#include "../RenderPipeline/Renderer.h"
#include "../Material/MaterialManager.h"
#include "../Mesh/MeshManager.h"
#include "../../IdTech4/MapFileParser.h"
#include "../../Global.h"
#include "../../Window.h"

#define ENT_NAME_PLAYER_START "info_player_start"
#define ENT_NAME_LIGHT "light"

namespace Framework 
{
	void CreateRenderersWithMapFile(IdTech4::MapFile* mapFile)
	{
		IdTech4::MapModelPtrVec* models = mapFile->GetModels();
		size_t numModels = models->size();
		for (size_t m = 0; m < numModels; ++m)
		{
			IdTech4::MapModelPtr model = (*models)[m];
			Entity* ent = SceneManager::CreateEntity(model->GetName());
			IdTech4::MapModelSurfacePtrVec* surfaces = model->GetSurfaces();
			size_t numSurfaces = surfaces->size();
			for (size_t s = 0; s < numSurfaces; ++s)
			{
				IdTech4::MapModelSurfacePtr surface = (*surfaces)[s];
				IdTech4::MapModelSurfaceVertexVec* vertices = surface->GetVertices();
				size_t numVertices = vertices->size();
				IdTech4::MapModelSurfaceIndexVec* indices = surface->GetIndices();
				size_t numIndices = indices->size();
				//
				std::string materialUrl = ASSETS_BASE_PATH + surface->GetMaterialUrl() + ".mtl";
				//
				Renderer* renderer = ent->CreateRenderer();
				renderer->SetMaterialInstance(MaterialManager::CreateMaterialInstance(materialUrl));
				//
				XMFLOAT3* meshVertices = new XMFLOAT3[numVertices]();
				XMFLOAT3* meshNormals = new XMFLOAT3[numVertices]();
				XMFLOAT2* meshTexUV = new XMFLOAT2[numVertices]();
				UINT* meshIndices = new UINT[numIndices];
				XMFLOAT3 bboxMax = { NAGETIVE_INFINITY,NAGETIVE_INFINITY,NAGETIVE_INFINITY };
				XMFLOAT3 bboxMin = { POSITIVE_INFINITY,POSITIVE_INFINITY,POSITIVE_INFINITY };
				for (size_t v = 0; v < numVertices; ++v)
				{
					IdTech4::MapModelSurfaceVertex vertex = (*vertices)[v];
					XMFLOAT3 pos = { vertex.x,vertex.z,vertex.y };
					XMFLOAT3 normal = { vertex.nx,vertex.nz,vertex.ny };
					XMFLOAT2 uv = { vertex.u,vertex.v };
					bboxMax.x = pos.x > bboxMax.x ? pos.x : bboxMax.x;
					bboxMax.y = pos.y > bboxMax.y ? pos.y : bboxMax.y;
					bboxMax.z = pos.z > bboxMax.z ? pos.z : bboxMax.z;
					bboxMin.x = pos.x < bboxMin.x ? pos.x : bboxMin.x;
					bboxMin.y = pos.y < bboxMin.y ? pos.y : bboxMin.y;
					bboxMin.z = pos.z < bboxMin.z ? pos.z : bboxMin.z;
					meshVertices[v] = pos;
					meshNormals[v] = normal;
					meshTexUV[v] = uv;
				}
				for (size_t i = 0; i < numIndices; ++i)
				{
					meshIndices[i] = (*indices)[i];
				}
				XNA::AxisAlignedBox aabb;
				aabb.Extents = (bboxMax - bboxMin) * 0.5f;
				aabb.Center = bboxMin + aabb.Extents;
				//
				Mesh* mesh = MeshManager::CreateMeshInstance();
				mesh->SetVertexData(numVertices, meshVertices);
				mesh->SetIndexData(numIndices, meshIndices);
				mesh->SetNormalData(meshNormals);
				mesh->SetUVData(meshTexUV);
				mesh->SetBoundingShape(aabb);
				mesh->UpLoad();
				renderer->SetMeshInstance(mesh);
			}
		}
	}

	void CreateEntitiesWithMapFile(IdTech4::MapFile* mapFile)
	{
		IdTech4::MapEntityPtrVec* entites = mapFile->GetEntities();
		size_t numEntites = entites->size();
		std::string entClassName;
		for (size_t e = 0; e < numEntites; ++e)
		{
			IdTech4::MapEntityPtr ent = (*entites)[e];
			entClassName = ent->GetClassName();
			if (entClassName == ENT_NAME_PLAYER_START)
			{
				XMFLOAT3 pos = ent->GetOrigin();
				Camera* mainCamera = SceneManager::CreateDefaultMainCamera();
				mainCamera->GetTransform()->position = pos;
				float angle = ent->GetFloat("angle");
				mainCamera->GetTransform()->rotation = { 0,angle,0 };
			}
			else if (entClassName == ENT_NAME_LIGHT)
			{
				XMFLOAT3 pos = ent->GetOrigin();
				XMFLOAT3 color = ent->GetFloat3("_color");
				XMFLOAT3 radius = ent->GetFloat3("light_radius");
				Light* pointLit = SceneManager::CreateLight(LIGHT_TYPE_POINT, ent->GetName());
				pointLit->SetIntensity(1)
					->SetColor(RGBA32{ color.x,color.y,color.z,1 })
					->SetRange(max(radius.x, max(radius.y, radius.z)))
					;
				pointLit->GetTransform()->position = pos;
			}
		}
	}

	bool CreateSceneFromIdTech4MapFile(const std::string& fileUrl)
	{
		IdTech4::MapFile* mapFile = new IdTech4::MapFile();
		if (IdTech4::MapFileParser::LoadAndParser(fileUrl, mapFile))
		{
			CreateRenderersWithMapFile(mapFile);
			CreateEntitiesWithMapFile(mapFile);
			return true;
		}
		return false;
	}
}