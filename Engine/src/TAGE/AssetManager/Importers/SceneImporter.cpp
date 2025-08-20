#include "tagepch.h"
#include "SceneImporter.h"
#include "TAGE/MultiThread/MultiThread.h"
#include "TAGE/Project/Project.h"
#include "TAGE/World/Scene/SceneSerializer.h"

namespace TAGE {
	MEM::Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		PROFILE_FUNCTION();

		return LoadScene(Project::GetAssetDirectory() / metadata.FilePath);
	}

	MEM::Ref<Scene> SceneImporter::LoadScene(const std::filesystem::path& filepath)
	{
		PROFILE_FUNCTION();
		MEM::Ref<Scene> scene = MEM::MakeRef<Scene>(filepath.stem().string());
		SceneSerializer serializer(scene);
		if (serializer.Deserialize(filepath))
		{
			LOG_INFO("SceneImporter Successfully loaded scene from: {}", filepath.string());
			return scene;
		}
		LOG_ERROR("SceneImporter Failed to load scene from: {}", filepath.string());
	}
}