#pragma once
#include "TAGE/Common/TTypes.h"
#include "TAGE/Renderer/SkinnedModel/Skeleton.h"
#include "Animation.h"

namespace TAGE::Renderer {
    class Animator {
    public:
        Animator(const Skeleton* skeleton) : _skeleton(skeleton), _finalBoneMatrices(skeleton->GetBoneCount(), glm::mat4(1.0f)) {}

        void PlayAnimation(Animation* animation) {
            _animation = animation;
            _currentTime = 0.0f;
        }

        void Update(float deltaTime);

        const std::vector<glm::mat4>& GetFinalBoneMatrices() const {
            return _finalBoneMatrices;
        }

    private:
        const Skeleton* _skeleton = nullptr;
        Animation* _animation = nullptr;
        float _currentTime = 0.0f;

        std::vector<glm::mat4> _finalBoneMatrices;

        void CalculateBoneTransform(int index, const glm::mat4& parentTransform);
    };
}