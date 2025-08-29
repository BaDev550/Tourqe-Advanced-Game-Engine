#pragma once

#include "TAGE/Common/TTypes.h"
#include <vector>
#include <assimp/scene.h>
#include <list>

namespace TARE {
	class Bone
	{
	public:
		Bone(const std::string& name, int id, const aiNodeAnim* channel);
		void Update(float animationTime);

		glm::mat4 GetLocalTransform() const { return _animation.LocalTransform; }
		const std::string& GetName() const { return _animation.BoneName; }
		int GetID() const { return _animation.ID; }
		int GetPositionIndex(float animationTime) const;
		int GetRotationIndex(float animationTime) const;
		int GetScaleIndex(float animationTime) const;
	private:
		BoneAnimation _animation;
	};
}

