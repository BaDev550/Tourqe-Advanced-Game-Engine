#include "tagepch.h"
#include "Animator.h"

namespace TARE {
	Animator::Animator(TAGE::MEM::Ref<Animation>& animation)
	{
		_CurrentTime = 0.0;
		_CurrentAnimation = animation;

		_FinalBoneMatrices.reserve(124);

		for (int i = 0; i < 124; i++)
			_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	void Animator::UpdateAnimation(float dt)
	{
		_DeltaTime = dt;
		if (_CurrentAnimation)
		{
			_CurrentTime += _CurrentAnimation->GetTicksPerSecond() * dt;
			_CurrentTime = fmod(_CurrentTime, _CurrentAnimation->GetDuration());
			CalculateBoneTransform(&_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}
	
	void Animator::PlayAnimation(TAGE::MEM::Ref<Animation>& pAnimation)
	{
		_CurrentAnimation = pAnimation;
		_CurrentTime = 0.0f;
	}

	void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* Bone = _CurrentAnimation->FindBone(nodeName);

		if (Bone) {
			Bone->Update(_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = _CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].Id;
			glm::mat4 offset = boneInfoMap[nodeName].Offset;
			_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> Animator::GetFinalBoneMatrices()
	{
		return _FinalBoneMatrices;
	}
}