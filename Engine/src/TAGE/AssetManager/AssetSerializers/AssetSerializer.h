//#pragma once
//#include "TAGE/Common/TEnums.h"
//#include <unordered_map>
//
//#include "TAGE/Renderer/Texture/Texture.h"
//#include "TAGE/Renderer/Model/Model.h"
//#include "TAGE/Utilities/UUID.h"
//
//namespace TAGE {
//	using namespace Renderer;
//#define MODEL_MAGIC 0x4D4F444C // 'MODL'
//#define MATERIAL_MAGIC 0x4D41544C // 'MATL'
//
//	struct Version {
//		uint8 major = ENGINE_VERSION_MAJOR;
//		uint8 minor = ENGINE_VERSION_MINOR;
//	};
//
//	struct ModelFileHeader {
//		int magic = MODEL_MAGIC;
//		Version version;
//		uint meshCount;
//	};
//
//	struct MeshFileHeader {
//		uint vertexCount;
//		uint indexCount;
//	};
//
//	struct MaterialFileHeader {
//		int magic = MATERIAL_MAGIC; 
//		Version version;
//		uint textureCount;
//	};
//
//	class ModelSerializer {
//	public:
//		static bool Serialize(Model& model, const std::string& outPath);
//		static bool Deserialize(Model& model, const std::string& inPath);
//	};
//
//	class MaterialSerializer {
//	public:
//		static bool Serialize(const Material& material, const std::string& outPath);
//		static bool Deserialize(Material& material, const std::string& inPath);
//	};
//
//	class TextureSerializer {
//	public:
//		static bool Serialize() {}
//		static bool Deserialize() {}
//	};
//}