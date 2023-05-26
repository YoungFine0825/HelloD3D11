
#include <unordered_map>
#include <istream>
#include <fstream>

#include "Mesh.h"
#include "MeshManager.h"
#include "MeshManager.h"
#include "tiny_obj_loader.h"

#include "MeshUtil.h"

namespace Framework 
{
	namespace MeshManager
	{
		typedef std::unordered_map<Resource::ResourceUrl, Mesh*> MeshResMap;
		MeshResMap m_meshResMap;

		typedef std::unordered_map<Resource::ResourceGUID, Mesh*> MeshInstanceMap;
		MeshInstanceMap m_meshInstanceMap;

		Mesh* FindWithUrl(Resource::ResourceUrl url)
		{
			MeshResMap::iterator it = m_meshResMap.find(url);
			if (it == m_meshResMap.end()) 
			{
				return nullptr;
			}
			return it->second;
		}

		Mesh* LoadFromTxtFile(Resource::ResourceUrl url) 
		{
			std::ifstream fin(url);
			if (!fin)
			{
				return nullptr;
			}

			Mesh* mesh = FindWithUrl(url);
			if (mesh == nullptr)
			{
				mesh = new Mesh();
				m_meshResMap[url] = mesh;
			}

			UINT vcount = 0;
			UINT tcount = 0;
			std::string ignore;

			fin >> ignore >> vcount;
			fin >> ignore >> tcount;
			fin >> ignore >> ignore >> ignore >> ignore;

			XMFLOAT3 vertex;
			XMFLOAT3 normal;
			XMFLOAT3 bboxMax = { NAGETIVE_INFINITY,NAGETIVE_INFINITY,NAGETIVE_INFINITY };
			XMFLOAT3 bboxMin = { POSITIVE_INFINITY,POSITIVE_INFINITY,POSITIVE_INFINITY };

			mesh->ClearVertices();
			mesh->ReserveVertices(vcount);

			for (UINT i = 0; i < vcount; ++i)
			{
				fin >> vertex.x >> vertex.y >> vertex.z;
				fin >> normal.x >> normal.y >> normal.z;
				//
				bboxMax.x = vertex.x > bboxMax.x ? vertex.x : bboxMax.x;
				bboxMax.y = vertex.y > bboxMax.y ? vertex.y : bboxMax.y;
				bboxMax.z = vertex.z > bboxMax.z ? vertex.z : bboxMax.z;
				bboxMin.x = vertex.x < bboxMin.x ? vertex.x : bboxMin.x;
				bboxMin.y = vertex.y < bboxMin.y ? vertex.y : bboxMin.y;
				bboxMin.z = vertex.z < bboxMin.z ? vertex.z : bboxMin.z;
				//
				MeshVertexDataPtr vertexData = mesh->CreateVertex();
				vertexData->x = vertex.x;
				vertexData->y = vertex.y;
				vertexData->z = vertex.z;
				vertexData->nx = normal.x;
				vertexData->ny = normal.y;
				vertexData->nz = normal.z;
				vertexData->r = 1.0f;
				vertexData->g = 1.0f;
				vertexData->b = 1.0f;
				vertexData->a = 1.0f;
			}

			fin >> ignore;
			fin >> ignore;
			fin >> ignore;

			UINT indicesNum = 3 * tcount;
			mesh->ClearVertexIndices();
			mesh->ReserveVertexIndices(indicesNum);
			MeshVertexIndex index0, index1, index2 = 0;
			for (UINT i = 0; i < tcount; ++i)
			{
				fin >> index0 >> index1 >> index2;
				mesh->AddVertexIndex(index0);
				mesh->AddVertexIndex(index1);
				mesh->AddVertexIndex(index2);
			}

			fin.close();

			mesh->UpLoad();
			//
			XNA::AxisAlignedBox aabb;
			aabb.Extents = (bboxMax - bboxMin) * 0.5f;
			aabb.Center = bboxMin + aabb.Extents;
			mesh->SetBoundingShape(aabb);
			//
			return mesh;
		}

		Mesh* LoadFromObjFile(Resource::ResourceUrl url) 
		{
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::string warn;
			std::string err;
			bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, url.c_str());
			if (!ret) 
			{
				return nullptr;
			}
			//
			UINT shapeCnt = shapes.size();
			UINT indicesNum = 0;
			for (size_t i = 0; i < shapeCnt; ++i) 
			{
				indicesNum += shapes[i].mesh.indices.size();
			}
			UINT vcount = indicesNum;
			//
						//
			Mesh* mesh = FindWithUrl(url);
			if (mesh == nullptr)
			{
				mesh = new Mesh();
				m_meshResMap[url] = mesh;
			}
			mesh->ClearVertices();
			mesh->ClearVertexIndices();
			mesh->ReserveVertices(vcount);
			mesh->ReserveVertexIndices(indicesNum);
			//
			XMFLOAT3 bboxMax = { NAGETIVE_INFINITY,NAGETIVE_INFINITY,NAGETIVE_INFINITY };
			XMFLOAT3 bboxMin = { POSITIVE_INFINITY,POSITIVE_INFINITY,POSITIVE_INFINITY };
			//
			UINT curIndicesIdx = 0;
			UINT curVertexIdx = 0;
			bool hasNormal = false;
			for (size_t s = 0; s < shapes.size(); s++) {
				// Loop over faces(polygon)
				size_t index_offset = 0;
				for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
					size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
					hasNormal = false;
					// Loop over vertices in the face.
					for (size_t v = 0; v < fv; v++) {
						// access to vertex
						tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
						//
						float vx = static_cast<float>(attrib.vertices[3 * size_t(idx.vertex_index) + 0]);
						float vy = static_cast<float>(attrib.vertices[3 * size_t(idx.vertex_index) + 1]);
						float vz = static_cast<float>(attrib.vertices[3 * size_t(idx.vertex_index) + 2]);
						//
						MeshVertexDataPtr vertexData = mesh->CreateVertex();
						vertexData->x = vx;
						vertexData->y = vy;
						vertexData->z = vz;
						//
						mesh->AddVertexIndex(curVertexIdx);
						//
						bboxMax.x = vx > bboxMax.x ? vx : bboxMax.x;
						bboxMax.y = vy > bboxMax.y ? vy : bboxMax.y;
						bboxMax.z = vz > bboxMax.z ? vz : bboxMax.z;
						bboxMin.x = vx < bboxMin.x ? vx : bboxMin.x;
						bboxMin.y = vy < bboxMin.y ? vy : bboxMin.y;
						bboxMin.z = vz < bboxMin.z ? vz : bboxMin.z;
						// Check if `normal_index` is zero or positive. negative = no normal data
						if (idx.normal_index >= 0) {
							auto nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
							auto ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
							auto nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
							vertexData->nx = static_cast<float>(nx);
							vertexData->ny = static_cast<float>(ny);
							vertexData->nz = static_cast<float>(nz);
							hasNormal = true;
						}
						// Check if `texcoord_index` is zero or positive. negative = no texcoord data
						if (idx.texcoord_index >= 0) 
						{
							auto tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
							auto ty = 1 - attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
							vertexData->u = static_cast<float>(tx);
							vertexData->v = static_cast<float>(ty);
						}
						else 
						{
							vertexData->u = 0.5f;
							vertexData->v = 0.5f;
						}
						//
						curIndicesIdx++;
						curVertexIdx++;
					}
					if (!hasNormal) 
					{
						UINT startVIdx = curVertexIdx - 2;
					}
					index_offset += fv;
				}
			}
			//¼ÆËãÇÐÏß
			Framework::MeshUtil::ComputeMeshTangents(mesh,vcount,indicesNum);
			//
			mesh->UpLoad();
			//
			XNA::AxisAlignedBox aabb;
			aabb.Extents = (bboxMax - bboxMin) * 0.5f;
			aabb.Center = bboxMin + aabb.Extents;
			mesh->SetBoundingShape(aabb);
			//
			shapes.clear();
			materials.clear();
			return mesh;
		}

		void Cleanup() 
		{
			MeshResMap::iterator it;
			for (it = m_meshResMap.begin(); it != m_meshResMap.end(); ++it) 
			{
				Resource::ReleaseWithGUID(it->second->GetGUID());
			}
			m_meshResMap.clear();
			//
			MeshInstanceMap::iterator it2;
			for (it2 = m_meshInstanceMap.begin(); it2 != m_meshInstanceMap.end(); ++it2)
			{
				Resource::ReleaseWithGUID(it2->second->GetGUID());
			}
			m_meshInstanceMap.clear();
		}

		Mesh* CreateQuad() 
		{
			Mesh* mesh = FindWithUrl("Quad");
			if (mesh != nullptr)
			{
				return mesh;
			}
			float halfSize = 1;
			mesh = new Mesh();
			//
			mesh->AddVertex({ halfSize * -1,halfSize,0,			0,0,-1,	0,0 })
				->AddVertex({ halfSize ,halfSize,0,				0,0,-1,	1,0 })
				->AddVertex({ halfSize * -1,halfSize * -1,0 ,	0,0,-1,	0,1 })
				->AddVertex({ halfSize ,halfSize * -1,0 ,		0,0,-1,	1 ,1 })
				;
			mesh->AddVertexIndex(0)
				->AddVertexIndex(1)
				->AddVertexIndex(2)
				->AddVertexIndex(2)
				->AddVertexIndex(1)
				->AddVertexIndex(3)
				;
			mesh->UpLoad();
			//
			XNA::AxisAlignedBox aabb;
			aabb.Extents = {1,1,0.01};
			aabb.Center = {0,0,0};
			mesh->SetBoundingShape(aabb);
			m_meshResMap["Quad"] = mesh;
			return mesh;
		}


		Mesh* CreateFullScreenTriangle() 
		{
			Resource::ResourceUrl url = "FullScreenTriangle";
			Mesh* mesh = FindWithUrl(url);
			if (mesh != nullptr) 
			{
				return mesh;
			}
			mesh = new Mesh();
			//
			mesh->AddVertex({ 0,0,0,	0,0,1,	0,0 })
				->AddVertex({ 2 ,0,0,	0,0,1,	2,0 })
				->AddVertex({ 0,-2,0 ,	0,0,1,	0,2 })
				;
			mesh->AddVertexIndex(0)
				->AddVertexIndex(1)
				->AddVertexIndex(2)
				;
			mesh->UpLoad();
			m_meshResMap[url] = mesh;
			return mesh;
		}

		Mesh* CreateMeshInstance() 
		{
			Mesh* m = new Mesh();
			Resource::ResourceGUID guid = m->GetGUID();
			m_meshInstanceMap[guid] = m;
			return m;
		}

		void DestroyMeshInstance(Mesh* meshInstance) 
		{
			if (!meshInstance) 
			{
				return;
			}
			DestroyMeshInstance(meshInstance->GetGUID());
		}

		void DestroyMeshInstance(Resource::ResourceGUID guid) 
		{
			MeshInstanceMap::iterator it = m_meshInstanceMap.find(guid);
			if (it == m_meshInstanceMap.end())
			{
				return;
			}
			m_meshInstanceMap.erase(it);
			Resource::ReleaseWithGUID(guid);
		}
	}
}