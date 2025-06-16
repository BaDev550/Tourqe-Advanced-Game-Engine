#include "tagepch.h"
//#include "AssetSerializer.h"
//#include "../AssetManager.h"
//
//namespace TAGE {
//	bool ModelSerializer::Serialize(Model& model, const std::string& outPath){
//		std::ofstream out(outPath, std::ios::binary);
//		ASSERT_NOMSG(out);
//
//		ModelFileHeader header;
//		header.meshCount = model.GetMeshCount();
//		out.write(reinterpret_cast<const char*>(&header), sizeof(header));
//
//		LOG_TRACE("Loading Model - Mesh Count: {}", model.GetMeshCount());
//		for (uint32_t i = 0; i < header.meshCount; ++i) {
//			Mesh& mesh = model.GetMesh(i);
//			const auto& vertices = mesh.GetVertices();
//			const auto& indices = mesh.GetIndices();
//			auto material = mesh.GetMaterial();
//
//			MeshFileHeader meshHeader;
//			meshHeader.vertexCount = static_cast<uint32_t>(vertices.size());
//			meshHeader.indexCount = static_cast<uint32_t>(indices.size());
//
//			//if (material) {
//			//	std::string materialPath = AssetManager::GetAssetOutputPath(material->GetName(), "Materials", ".material");
//			//	uint32_t pathLen = static_cast<uint32_t>(materialPath.length());
//			//	out.write(reinterpret_cast<const char*>(&pathLen), sizeof(pathLen));
//			//	out.write(materialPath.c_str(), pathLen);
//			//} 
//			//else {
//			//	uint32_t pathLen = 0;
//			//	out.write(reinterpret_cast<const char*>(&pathLen), sizeof(pathLen));
//			//}
//
//			out.write(reinterpret_cast<const char*>(&meshHeader), sizeof(meshHeader));
//			out.write(reinterpret_cast<const char*>(vertices.data()), vertices.size() * sizeof(VertexData));
//			out.write(reinterpret_cast<const char*>(indices.data()), indices.size() * sizeof(uint32_t));
//		}
//		return true;
//	}
//
//	bool ModelSerializer::Deserialize(Model& model, const std::string& inPath) {
//		std::ifstream in(inPath, std::ios::binary);
//		if (!in) return false;
//
//		ModelFileHeader header;
//		in.read(reinterpret_cast<char*>(&header), sizeof(header));
//		if (header.magic != MODEL_MAGIC) { LOG_CRITICAL("Unsupported file format"); return false; }
//		if (header.version.major != ENGINE_VERSION_MAJOR || header.version.minor != ENGINE_VERSION_MINOR) { LOG_CRITICAL("Asset is not updated"); return false; }
//
//		for (uint32_t i = 0; i < header.meshCount; ++i) {
//			MeshFileHeader meshHeader;
//			in.read(reinterpret_cast<char*>(&meshHeader), sizeof(meshHeader));
//
//			std::vector<VertexData> vertices;
//			vertices.reserve(meshHeader.vertexCount);
//			for (uint32_t v = 0; v < meshHeader.vertexCount; ++v) {
//				VertexData vd;
//				in.read(reinterpret_cast<char*>(&vd), sizeof(VertexData));
//				vertices.push_back(vd);
//			}
//
//			std::vector<uint32_t> indices;
//			indices.reserve(meshHeader.indexCount);
//			for (uint32_t idx = 0; idx < meshHeader.indexCount; ++idx) {
//				uint32_t index;
//				in.read(reinterpret_cast<char*>(&index), sizeof(uint32_t));
//				indices.push_back(index);
//			}
//
//			MEM::Scope<Material> mat = MEM::MakeScope<Material>(inPath.c_str(), "MainShader");
//			MEM::Scope<Mesh> mesh = MEM::MakeScope<Mesh>(std::move(vertices), std::move(indices), std::move(mat));
//			model.AddMesh(std::move(mesh));
//		}
//
//		LOG_TRACE("Model - {} is loaded", inPath);
//		LOG_TRACE("	Version Major - {}", header.version.major);
//		LOG_TRACE("	Version Minor - {}", header.version.minor);
//		LOG_TRACE("	Mesh Count - {}", header.meshCount);
//
//		return true;
//	}
//}