#include "IdTech4SceneCreator.h"
#include "SceneManager.h"
#include "Entity.h"
#include "Camera.h"
#include "../RenderPipeline/Renderer.h"
#include "../Material/MaterialManager.h"
#include "../Mesh/MeshManager.h"
#include "../Mesh/MeshUtil.h"
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
				Mesh* mesh = MeshManager::CreateMeshInstance();
				mesh->ReserveVertices(numVertices);
				mesh->ReserveVertexIndices(numIndices);
				XMFLOAT3 bboxMax = { NAGETIVE_INFINITY,NAGETIVE_INFINITY,NAGETIVE_INFINITY };
				XMFLOAT3 bboxMin = { POSITIVE_INFINITY,POSITIVE_INFINITY,POSITIVE_INFINITY };
				for (size_t v = 0; v < numVertices; ++v)
				{
					IdTech4::MapModelSurfaceVertex vertex = (*vertices)[v];
					bboxMax.x = vertex.x > bboxMax.x ? vertex.x : bboxMax.x;
					bboxMax.y = vertex.z > bboxMax.y ? vertex.z : bboxMax.y;
					bboxMax.z = vertex.y > bboxMax.z ? vertex.y : bboxMax.z;
					bboxMin.x = vertex.x < bboxMin.x ? vertex.x : bboxMin.x;
					bboxMin.y = vertex.z < bboxMin.y ? vertex.z : bboxMin.y;
					bboxMin.z = vertex.y < bboxMin.z ? vertex.y : bboxMin.z;
					MeshVertexDataPtr vertexData = mesh->CreateVertex();
					vertexData->x = vertex.x;
					vertexData->y = vertex.z;
					vertexData->z = vertex.y;
					vertexData->nx = vertex.nx;
					vertexData->ny = vertex.nz;
					vertexData->nz = vertex.ny;
					vertexData->u = vertex.u;
					vertexData->v = vertex.v;
				}
				for (size_t i = 0; i < numIndices; ++i)
				{
					mesh->AddVertexIndex((*indices)[i]);
				}
				XNA::AxisAlignedBox aabb;
				aabb.Extents = (bboxMax - bboxMin) * 0.5f;
				aabb.Center = bboxMin + aabb.Extents;
				//
				mesh->SetBoundingShape(aabb);
				//¼ÆËãÇÐÏß
				Framework::MeshUtil::ComputeMeshTangents(mesh,numVertices,numIndices);
				//
				mesh->UpLoad();
				renderer->SetMeshInstance(mesh);
			}
		}
	}

	XMFLOAT3 MatrixToEulerAngle(XMMATRIX* matrix)
	{
		XMFLOAT3 ret = { 0,0,0 };
		float h, p, b = 0;
		float sp = -matrix->_32;
		if (sp <= -1.0f)
		{
			p = -1.570796f;
		}
		else if (sp >= 1.0f)
		{
			p = 1.570796f;
		}
		else
		{
			p = asin(sp);
		}
		if (fabs(sp) > 0.9999f)
		{
			b = 0.0f;
			h = atan2(-matrix->_13, -matrix->_11);
		}
		else
		{
			h = atan2(matrix->_31, matrix->_33);
			b = atan2(matrix->_12, matrix->_22);
		}
		ret.x = Radin2Angle(p);
		ret.y = Radin2Angle(h);
		ret.z = Radin2Angle(b);
		return ret;
	}

	LIGHT_TYPE DetermineLightType(IdTech4::MapEntityPtr ent) 
	{
		if (ent->haveKey("light_radius")) 
		{
			return LIGHT_TYPE_POINT;
		}
		bool isParallel = ent->GetBool("parallel");
		return isParallel ? LIGHT_TYPE_DIRECTIONAL : LIGHT_TYPE_SPOT;
	}

	void ComputeLightRotationW(IdTech4::MapEntityPtr ent,XMFLOAT3 litTarget, XMFLOAT3 litRight, XMFLOAT3 litUp, XMFLOAT3* angleEuler)
	{
		XMMATRIX rot = XMMatrixIdentity();
		ent->GetMatrix("rotation", &rot);
		float t = 0;
		t = litTarget.y;	litTarget.y = litTarget.z;	litTarget.z = t;
		t = litRight.y;		litRight.y = litRight.z;	litRight.z = t;
		t = litUp.y;		litUp.y = litUp.z;			litUp.z = t;
		XMMATRIX defaultOri = XMMatrixIdentity();
		defaultOri._11 = litRight.x;	defaultOri._12 = litRight.y;	defaultOri._13 = litRight.z;
		defaultOri._21 = litUp.x;		defaultOri._22 = litUp.y;		defaultOri._23 = litUp.z;
		defaultOri._31 = litTarget.x;	defaultOri._32 = litTarget.y;	defaultOri._33 = litTarget.z;
		XMFLOAT3 defaultLitTarget = XMFloat3MultiMatrix({ 0,0,1 }, defaultOri);
		XMFLOAT3 litOrient = XMFloat3MultiMatrix(XMVectorNormalize(defaultLitTarget), rot);
		XMFLOAT3 right = XMVectorNormalize(XMVectorCross({ 0,1,0 }, litOrient));
		XMFLOAT3 up = XMVectorNormalize(XMVectorCross(litOrient, right));
		XMMATRIX orient = XMMatrixIdentity();
		orient._11 = right.x;		orient._12 = right.y;		orient._13 = right.z;
		orient._21 = up.x;			orient._22 = up.y;			orient._23 = up.z;
		orient._31 = litOrient.x;	orient._32 = litOrient.y;	orient._33 = litOrient.z;
		(*angleEuler) = MatrixToEulerAngle(&orient);
	}

	void CreateLight(IdTech4::MapEntityPtr ent) 
	{
		XMFLOAT3 pos = ent->GetOrigin();
		XMFLOAT3 color = ent->GetFloat3("_color");
		XMFLOAT3 radius = ent->GetFloat3("light_radius");

		bool isParallel = ent->GetBool("parallel");
		LIGHT_TYPE litType = DetermineLightType(ent);
		Light* lit = SceneManager::CreateLight(litType, ent->GetName());
		lit->SetIntensity(1)
			->SetColor(RGBA32{ color.x,color.y,color.z,1 })
			->SetRange(max(radius.x, max(radius.y, radius.z)))
			;
		lit->GetTransform()->position = pos;
		//
		if (litType != LIGHT_TYPE_POINT) 
		{
			XMFLOAT3 light_target = ent->GetFloat3("light_target");
			XMFLOAT3 light_right = ent->GetFloat3("light_right");
			XMFLOAT3 light_up = ent->GetFloat3("light_up");
			float range = XMFloat3Length(light_target);
			lit->SetRange(range);
			float radius = min(light_right.x, light_up.y);
			float slope = sqrtf(range * range + radius * radius);
			float spot = Radin2Angle(asin(radius / slope));
			lit->SetSpot(spot);
			//
			XMFLOAT3 rotEuler = {0,0,0};
			ComputeLightRotationW(ent, light_target, light_right, light_up, &rotEuler);
			lit->GetTransform()->rotation = rotEuler;
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
				//mainCamera->GetTransform()->rotation = { 0,angle,0 };
			}
			else if (entClassName == ENT_NAME_LIGHT)
			{
				CreateLight(ent);
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