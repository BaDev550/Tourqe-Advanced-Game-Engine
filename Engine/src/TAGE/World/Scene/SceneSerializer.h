#pragma once
#include "Scene.h"

namespace TAGE {
	class SceneSerializer
	{
	public:
		SceneSerializer(const MEM::Ref<Scene>& scene);

		void Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);
	private:
		MEM::Ref<Scene> _Scene;
	};
}