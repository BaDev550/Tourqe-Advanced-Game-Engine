#include "tagepch.h"
#include "Animator.h"

namespace TAGE::Renderer {
	void Animator::Update(float deltaTime)
	{
        if (!_animation || !_skeleton) return;

        _currentTime += deltaTime * _animation->TicksPerSecond;
        _currentTime = fmod(_currentTime, _animation->Duration);

        /*const auto& rootBoneIndices = */
        CalculateBoneTransform(0, glm::mat4(1.0f));
	}

	void Animator::CalculateBoneTransform(int index, const glm::mat4& parentTransform)
	{
        const Bone& bone = _skeleton->GetBones()[index];
        glm::mat4 localTransform = bone.LocalTransform;

        if (const BoneAnimation* anim = _animation->GetBoneAnim(bone.Name)) {
            localTransform = anim->Interpolate(_currentTime);
        }

        glm::mat4 globalTransform = parentTransform * localTransform;

        _finalBoneMatrices[index] = globalTransform * bone.OffsetMatrix;

        for (int childIndex : bone.ChildrenIndices) {
            CalculateBoneTransform(childIndex, globalTransform);
        }
	}
}