#pragma once
#include "TAGE/Utilities/Memory.h"
#include "Animation.h"
#include "Bone.h"

namespace TARE {
	class Animator
	{
	public:
		Animator(TAGE::MEM::Ref<Animation>& animation);
		void UpdateAnimation(float dt);
		void PlayAnimation(TAGE::MEM::Ref<Animation>& pAnimation);
		void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

		std::vector<glm::mat4> GetFinalBoneMatrices();

	private:
		std::vector<glm::mat4> _FinalBoneMatrices;
		TAGE::MEM::Ref<Animation> _CurrentAnimation;
		float _CurrentTime;
		float _DeltaTime;
	};
}