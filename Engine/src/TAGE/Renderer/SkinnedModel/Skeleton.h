#pragma once
#include "TAGE/Common/TTypes.h"

namespace TAGE::Renderer {
	class Skeleton
	{
	public:
		void AddBone(const std::string& name, int parentIndex, const glm::mat4& offsetMatrix);
		const std::vector<Bone>& GetBones() const;

		int GetBoneIndex(const std::string& name) const;

		size_t GetBoneCount() const { return _bones.size(); }
	private:
		std::vector<Bone> _bones;
		std::unordered_map<std::string, int> _boneNameToIndex;
	};
}