#include "tagepch.h"
#include "Animator.h"

namespace TAGE::Renderer {
	void Animator::Update(float deltaTime)
	{
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

        for (int i = 0; i < _skeleton->GetBoneCount(); ++i) {
            if (_skeleton->GetBones()[i].ParentIndex == index) {
                CalculateBoneTransform(i, globalTransform);
            }
        }
	}
}