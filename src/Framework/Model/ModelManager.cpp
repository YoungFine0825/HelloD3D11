#include <unordered_map>

#include "ModelManager.h"
#include "Model.h"
#include "tiny_obj_loader.h"
#include "../Mesh/MeshUtil.h"

namespace Framework 
{
	namespace ModelManager 
	{
		typedef std::unordered_map<Resource::ResourceUrl, Model*> ModelResMap;
		ModelResMap m_modelResMap;

		Model* FindWithUrl(Resource::ResourceUrl url) 
		{
			ModelResMap::iterator it = m_modelResMap.find(url);
			if (it == m_modelResMap.end())
			{
				return nullptr;
			}
			return it->second;
		}


		Model* LoadFromObjFile(Resource::ResourceUrl url) 
		{
			Model* model = FindWithUrl(url);
			if (model != nullptr)
			{
				return model;
			}
			//
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::string warn;
			std::string err;
			bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, url.c_str());
			if (!ret)
			{
				shapes.clear();
				materials.clear();
				return nullptr;
			}
			//
			Model* newModel = new Model();
			//
			bool hasNormal = false;
			for (size_t s = 0; s < shapes.size(); s++) {
				//遍历ploygon
				size_t index_offset = 0;
				
				UINT indicesNum = shapes[s].mesh.indices.size();
				UINT vcount = indicesNum;
				//
				Mesh* newMesh = newModel->CreateMesh();
				newMesh->ReserveVertices(vcount);
				newMesh->ReserveVertexIndices(indicesNum);
				XMFLOAT3 bboxMax = { NAGETIVE_INFINITY,NAGETIVE_INFINITY,NAGETIVE_INFINITY };
				XMFLOAT3 bboxMin = { POSITIVE_INFINITY,POSITIVE_INFINITY,POSITIVE_INFINITY };
				UINT curIndicesIdx = 0;
				UINT curVertexIdx = 0;
				//
				for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
					//
					size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
					hasNormal = false;
					//遍历三角形
					for (size_t v = 0; v < fv; v++) {
						// access to vertex
						tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
						//
						float vx = static_cast<float>(attrib.vertices[3 * size_t(idx.vertex_index) + 0]);
						float vy = static_cast<float>(attrib.vertices[3 * size_t(idx.vertex_index) + 1]);
						float vz = static_cast<float>(attrib.vertices[3 * size_t(idx.vertex_index) + 2]);
						MeshVertexDataPtr vertex = newMesh->CreateVertex();
						vertex->x = vx;
						vertex->y = vy;
						vertex->z = vz;
						//
						newMesh->AddVertexIndex(curVertexIdx);
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
							XMFLOAT3 normal = XMVectorNormalize({ static_cast<float>(nx) ,static_cast<float>(ny),static_cast<float>(nz) });
							vertex->nx = normal.x;
							vertex->ny = normal.y;
							vertex->nz = normal.z;
							hasNormal = true;
						}
						// Check if `texcoord_index` is zero or positive. negative = no texcoord data
						if (idx.texcoord_index >= 0)
						{
							auto tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
							auto ty = 1 - attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
							vertex->u = static_cast<float>(tx);
							vertex->v = static_cast<float>(ty);
						}
						else
						{
							vertex->u = 0.5f;
							vertex->v = 0.5f;
						}
						//
						curIndicesIdx++;
						curVertexIdx++;
					}
					if (!hasNormal)
					{
						UINT startVIdx = curVertexIdx - 3;
						MeshVertexDataPtr vertex0 = newMesh->GetVertex(startVIdx);
						MeshVertexDataPtr vertex1 = newMesh->GetVertex(startVIdx + 1);
						MeshVertexDataPtr vertex2 = newMesh->GetVertex(startVIdx + 2);
						XMFLOAT3 v0 = XMFLOAT3{	vertex1->x,vertex1->y,vertex1->z } - XMFLOAT3{ vertex0->x, vertex0->y, vertex0->z };
						XMFLOAT3 v1 = XMFLOAT3{ vertex2->x,vertex2->y,vertex2->z } - XMFLOAT3{ vertex1->x, vertex1->y, vertex1->z };
						XMFLOAT3 n = XMVectorNormalize(XMVectorCross(v0, v1));
						vertex0->nx = n.x; vertex0->ny = n.y; vertex0->nz = n.z;
						vertex1->nx = n.x; vertex1->ny = n.y; vertex1->nz = n.z;
						vertex2->nx = n.x; vertex2->ny = n.y; vertex2->nz = n.z;
					}
					index_offset += fv;
				}
				//
				XNA::AxisAlignedBox aabb;
				aabb.Extents = (bboxMax - bboxMin) * 0.5f;
				aabb.Center = bboxMin + aabb.Extents;
				newMesh->SetBoundingShape(aabb);
				//计算切线
				Framework::MeshUtil::ComputeMeshTangents(newMesh, vcount, indicesNum);
			}
			//
			shapes.clear();
			materials.clear();
			//
			newModel->UploadMeshes();
			//
			m_modelResMap[url] = newModel;
			return newModel;
		}


		void Cleanup() 
		{
			ModelResMap::iterator it;
			for (it = m_modelResMap.begin(); it != m_modelResMap.end(); ++it)
			{
				Resource::ReleaseWithGUID(it->second->GetGUID());
			}
			m_modelResMap.clear();
		}
	}
}