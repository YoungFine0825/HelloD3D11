#include <unordered_map>

#include "ModelManager.h"
#include "Model.h"
#include "tiny_obj_loader.h"

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
				Mesh* newMesh = newModel->CreateMesh();
				UINT indicesNum = shapes[s].mesh.indices.size();
				UINT vcount = indicesNum;
				//
				XMFLOAT3* vertices = new XMFLOAT3[vcount]();
				XMFLOAT3* normals = new XMFLOAT3[vcount]();
				XMFLOAT2* texCoord = new XMFLOAT2[vcount]();
				UINT* indices = new UINT[indicesNum];
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
						indices[curIndicesIdx] = curVertexIdx;
						//
						float vx = static_cast<float>(attrib.vertices[3 * size_t(idx.vertex_index) + 0]);
						float vy = static_cast<float>(attrib.vertices[3 * size_t(idx.vertex_index) + 1]);
						float vz = static_cast<float>(attrib.vertices[3 * size_t(idx.vertex_index) + 2]);
						vertices[curVertexIdx] = { vx,vy,vz };
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
							normals[curVertexIdx] = XMVectorNormalize({ static_cast<float>(nx) ,static_cast<float>(ny),static_cast<float>(nz) });
							hasNormal = true;
						}
						// Check if `texcoord_index` is zero or positive. negative = no texcoord data
						if (idx.texcoord_index >= 0)
						{
							auto tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
							auto ty = 1 - attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
							texCoord[curVertexIdx].x = static_cast<float>(tx);
							texCoord[curVertexIdx].y = static_cast<float>(ty);
						}
						else
						{
							texCoord[curVertexIdx].x = 0.5f;
							texCoord[curVertexIdx].y = 0.5f;
						}
						//
						curIndicesIdx++;
						curVertexIdx++;
					}
					if (!hasNormal)
					{
						UINT startVIdx = curVertexIdx - 3;
						XMFLOAT3 v0 = vertices[startVIdx + 1] - vertices[startVIdx];
						XMFLOAT3 v1 = vertices[startVIdx + 2] - vertices[startVIdx + 1];
						XMFLOAT3 n = XMVectorNormalize(XMVectorCross(v0, v1));
						normals[startVIdx] = n;
						normals[startVIdx + 1] = n;
						normals[startVIdx + 2] = n;
					}
					index_offset += fv;
				}
				//
				newMesh->SetVertexData(vcount, vertices);
				newMesh->SetIndexData(indicesNum, indices);
				newMesh->SetNormalData(normals);
				newMesh->SetUVData(texCoord);
				//
				XNA::AxisAlignedBox aabb;
				aabb.Extents = (bboxMax - bboxMin) * 0.5f;
				aabb.Center = bboxMin + aabb.Extents;
				newMesh->SetBoundingShape(aabb);
				//
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