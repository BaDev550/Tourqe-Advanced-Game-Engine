#include "tagepch.h"
#include "Skeleton.h"

namespace TAGE::Renderer {
    void Skeleton::AddBone(const std::string& name, int parentIndex, const glm::mat4& offsetMatrix)
    {
        Bone bone;
        bone.Name = name;
        bone.ParentIndex = parentIndex;
        bone.OffsetMatrix = offsetMatrix;
        _bones.push_back(bone);
        _boneNameToIndex[name] = static_cast<int>(_bones.size()) - 1;
    }
    const std::vector<Bone>& Skeleton::GetBones() const { return _bones; }

    int Skeleton::GetBoneIndex(const std::string& name) const
    {
        auto it = _boneNameToIndex.find(name);
        return (it != _boneNameToIndex.end()) ? it->second : -1;
    }
}