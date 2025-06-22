#pragma once
#include "Scene.h"

namespace TAGE {
	class SceneSerializer
	{
	public:
		SceneSerializer(const MEM::Ref<Scene>& scene);

		void Serialize(const std::string& filepath);
		void SerializeBinary(const std::string& filepath);

		bool Deserialize(const std::string& filepath);
		bool DeserializeBinary(const std::string& filepath);
	private:
		MEM::Ref<Scene> _Scene;
	};
}