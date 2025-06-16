#pragma once
#include "TAGE/Common/TTypes.h"
#include <string>

namespace TAGE::Renderer {
    class Animation {
    public:
        std::string Name;
        float Duration = 0.0f;
        float TicksPerSecond = 25.0f;
        std::unordered_map<std::string, BoneAnimation> BoneAnimations;

        const BoneAnimation* GetBoneAnim(const std::string& name) const {
            auto it = BoneAnimations.find(name);
            return (it != BoneAnimations.end()) ? &it->second : nullptr;
        }
    };
}