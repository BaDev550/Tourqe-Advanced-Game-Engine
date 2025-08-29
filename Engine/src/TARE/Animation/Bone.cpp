#include "tagepch.h"
#include "Bone.h"

namespace TARE {
	Bone::Bone(const std::string& name, int id, const aiNodeAnim* channel)
	{
		_animation.BoneName = name;
		_animation.ID = id;
		_animation.LocalTransform = glm::mat4(1.0f);
		_animation.NumPosition = channel->mNumPositionKeys;

		for (int positionIndex = 0; positionIndex < _animation.NumPosition; ++positionIndex)
		{
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
			float timeStamp = channel->mPositionKeys[positionIndex].mTime;
			Keyframe data;
			data.Position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
			data.Time = timeStamp;
			_animation.PositionKeys.push_back(data);
		}

		_animation.NumRotation = channel->mNumRotationKeys;
		for (int rotationIndex = 0; rotationIndex < _animation.NumRotation; ++rotationIndex)
		{
			aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
			Keyframe data;
			data.Rotation = glm::quat(aiOrientation.x, aiOrientation.y, aiOrientation.z, aiOrientation.w);
			data.Time = timeStamp;
			_animation.RotationKeys.push_back(data);
		}

		_animation.NumScale = channel->mNumScalingKeys;
		for (int keyIndex = 0; keyIndex < _animation.NumScale; ++keyIndex)
		{
			aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
			float timeStamp = channel->mScalingKeys[keyIndex].mTime;
			Keyframe data;
			data.Scale = glm::vec3(scale.x, scale.y, scale.z);
			data.Time = timeStamp;
			_animation.ScaleKeys.push_back(data);
		}
	}

	void Bone::Update(float animationTime) {
		_animation.LocalTransform = _animation.Interpolate(animationTime);
	}

	int Bone::GetPositionIndex(float animationTime) const
	{
		for (int index = 0; index < _animation.NumPosition - 1; ++index)
		{
			if (animationTime < _animation.PositionKeys[index + 1].Time)
				return index;
		}
		return 0;
	}

	int Bone::GetRotationIndex(float animationTime) const
	{
		for (int index = 0; index < _animation.NumRotation - 1; ++index)
		{
			if (animationTime < _animation.RotationKeys[index + 1].Time)
				return index;
		}
		return 0;
	}

	int Bone::GetScaleIndex(float animationTime) const
	{
		for (int index = 0; index < _animation.NumScale - 1; ++index)
		{
			if (animationTime < _animation.ScaleKeys[index + 1].Time)
				return index;
		}
		return 0;
	}
}