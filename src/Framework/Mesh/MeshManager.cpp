
#include <unordered_map>
#include <istream>
#include <fstream>

#include "Mesh.h"
#include "MeshManager.h"

#include "tiny_obj_loader.h"

namespace Framework 
{
	namespace MeshManager
	{
		typedef std::unordered_map<Resource::ResourceUrl, Mesh*> MeshResMap;
		MeshResMap m_meshResMap;

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

			UINT vcount = 0;
			UINT tcount = 0;
			std::string ignore;

			fin >> ignore >> vcount;
			fin >> ignore >> tcount;
			fin >> ignore >> ignore >> ignore >> ignore;

			XMFLOAT4 defaulColor(1.0f, 1.0f, 1.0f, 1.0f);

			XMFLOAT3* vertices = new XMFLOAT3[vcount]();
			XMFLOAT4* colors = new XMFLOAT4[vcount]();
			XMFLOAT3* normals = new XMFLOAT3[vcount]();
			for (UINT i = 0; i < vcount; ++i)
			{
				fin >> vertices[i].x >> vertices[i].y >> vertices[i].z;
				fin >> normals[i].x >> normals[i].y >> normals[i].z;
				colors[i] = defaulColor;
			}

			fin >> ignore;
			fin >> ignore;
			fin >> ignore;

			UINT indicesNum = 3 * tcount;
			UINT* indices = new UINT[indicesNum];
			for (UINT i = 0; i < tcount; ++i)
			{
				fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
			}

			fin.close();

			Mesh* mesh = FindWithUrl(url);
			if (mesh == nullptr)
			{
				mesh = new Mesh();
				m_meshResMap[url] = mesh;
			}
			mesh->SetVertexData(vcount, vertices);
			mesh->SetIndexData(indicesNum, indices);
			mesh->SetNormalData(normals);
			mesh->SetColorData(colors);
			mesh->UpLoad();
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
			XMFLOAT3* vertices = new XMFLOAT3[vcount]();
			XMFLOAT3* normals = new XMFLOAT3[vcount]();
			XMFLOAT2* texCoord = new XMFLOAT2[vcount]();
			UINT* indices = new UINT[indicesNum];
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
						indices[curIndicesIdx] = curVertexIdx;
						//
						auto vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
						auto vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
						auto vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
						vertices[curVertexIdx].x = static_cast<float>(vx);
						vertices[curVertexIdx].y = static_cast<float>(vy);
						vertices[curVertexIdx].z = static_cast<float>(vz);
						// Check if `normal_index` is zero or positive. negative = no normal data
						if (idx.normal_index >= 0) {
							auto nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
							auto ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
							auto nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
							normals[curVertexIdx].x = static_cast<float>(nx);
							normals[curVertexIdx].y = static_cast<float>(ny);
							normals[curVertexIdx].z = static_cast<float>(nz);
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
						UINT startVIdx = curVertexIdx - 2;
					}
					index_offset += fv;
				}
			}
			//
			Mesh* mesh = FindWithUrl(url);
			if (mesh == nullptr)
			{
				mesh = new Mesh();
				m_meshResMap[url] = mesh;
			}
			mesh->SetVertexData(vcount, vertices);
			mesh->SetIndexData(indicesNum, indices);
			mesh->SetNormalData(normals);
			mesh->SetUVData(texCoord);
			mesh->UpLoad();
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
		}

		Mesh* CreateQuad() 
		{
			Mesh* mesh = FindWithUrl("Quad");
			if (mesh != nullptr)
			{
				return mesh;
			}
			float halfSize = 1;
			XMFLOAT3* vertices = new XMFLOAT3[4]{
				{halfSize * -1,halfSize,0},
				{halfSize ,halfSize,0},
				{halfSize * -1,halfSize* -1,0},
				{halfSize ,halfSize * -1,0}
			};
			XMFLOAT3* normals = new XMFLOAT3[4]{
				{0,0,-1},{0,0,-1},
				{0,0,-1},{0,0,-1}
			};
			UINT* indices = new UINT[6]{
				0,1,2,
				2,1,3
			};
			XMFLOAT4* colors = new XMFLOAT4[4]{
				{1.0f,1.0f,1.0f,1.0f},
				{1.0f,1.0f,1.0f,1.0f},
				{1.0f,1.0f,1.0f,1.0f},
				{1.0f,1.0f,1.0f,1.0f}
			};
			XMFLOAT2* uvs = new XMFLOAT2[4]{
				{0,0},
				{1 ,0},
				{0,1},
				{1 ,1}
			};
			mesh = new Mesh();
			mesh->SetVertexData(4, vertices);
			mesh->SetIndexData(6, indices);
			mesh->SetNormalData(normals);
			mesh->SetColorData(colors);
			mesh->SetUVData(uvs);
			mesh->UpLoad();
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
			XMFLOAT3* vertices = new XMFLOAT3[3]{
				{0,0,0},{2 ,0,0},
				{0,-2,0}
			};
			XMFLOAT3* normals = new XMFLOAT3[3]{
				{0,0,0},{0,0,0},
				{0,0,0}
			};
			UINT* indices = new UINT[3]{
				0,1,2
			};
			XMFLOAT4* colors = new XMFLOAT4[3]{
				{1.0f,1.0f,1.0f,1.0f},
				{1.0f,1.0f,1.0f,1.0f},
				{1.0f,1.0f,1.0f,1.0f}
			};
			XMFLOAT2* uvs = new XMFLOAT2[3]{
				{0,0},{2 ,0},
				{0,2}
			};
			mesh = new Mesh();
			mesh->SetVertexData(3, vertices);
			mesh->SetIndexData(3, indices);
			mesh->SetNormalData(normals);
			mesh->SetColorData(colors);
			mesh->SetUVData(uvs);
			mesh->UpLoad();
			m_meshResMap[url] = mesh;
			return mesh;
		}
	}
}