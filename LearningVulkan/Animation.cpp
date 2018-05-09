#include "Animation.h"
#include "Utils.h"
#include <algorithm>
#include <stack>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\matrix_interpolation.hpp>
#include "GLMAssimp.h"
#include "Model.h"

Animation::Animation(const aiAnimation * animation, const aiScene * scene)
{
	std::vector<aiNodeAnim*> animationNodes;
	for (uint32_t i = 0; i < animation->mNumChannels; ++i)
	{
		animationNodeNames.push_back(animation->mChannels[i]->mNodeName.C_Str());
		animationNodes.push_back(animation->mChannels[i]);
	}
	//std::vector<Bone*> bones; //order actually processed in
	std::stack<std::pair<Bone*, aiNode*>> nodesToVisit;
	nodesToVisit.push(std::make_pair(nullptr, scene->mRootNode));
	while (nodesToVisit.size() > 0)
	{
		Bone* parentBone = nodesToVisit.top().first;
		aiNode* node = nodesToVisit.top().second;
		nodesToVisit.pop();
		std::string name = node->mName.C_Str();
		printf("checking %s...\n", name.c_str());
		auto find = std::find(animationNodeNames.begin(), animationNodeNames.end(), name);
		Bone* bone = nullptr;
		if (find != animationNodeNames.end())
		{
			uint32_t idx = std::distance(animationNodeNames.begin(), find);
			bone = new Bone(animationNodes[idx], idx, parentBone);
			bones.push_back(bone);
		}

		for (uint32_t i = 0; i < node->mNumChildren; ++i)
			nodesToVisit.push(std::make_pair(bone, node->mChildren[i]));
	}

	duration = animation->mDuration;
	tps = (float)(animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f);
}

std::vector<std::string> Animation::GetLayout()
{
	return animationNodeNames;
}

std::string Animation::GetBoneName(int i)
{
	return animationNodeNames[i];
}

//std::vector<std::pair<std::string, glm::mat4>> Animation::GetAnimationFrame(float time)
//{
//	time *= tps;
//	if (time > duration) time = fmod(time, duration);
//	printf("time: %lf\n", time);
//	std::vector<std::pair<std::string, glm::mat4>> mats(bones.size());
//	for (auto bone : bones)
//	{
//		glm::mat4 parentMat = (bone->GetParent()) ? mats[bone->GetParent()->GetID()].second : glm::mat4(1);
//		mats[bone->GetID()] = std::make_pair(bone->GetName(), parentMat * bone->GetMatrix(time));
//	}
//	return mats;
//}

void Animation::GetAnimationFrame(std::vector<glm::mat4*> boneMap, float time)
{
	time *= tps;
	if (time > duration) time = fmod(time, duration);
	//printf("time: %lf\n", time);
	for (auto bone : bones)
	{
		glm::mat4 parentMat = (bone->GetParent()) ? *boneMap[bone->GetParent()->GetID()] : glm::mat4(1);
		*boneMap[bone->GetID()] = bone->GetMatrix(time) * parentMat;
	}
}

KeyFrame* Bone::GetOrCreateKeyFrame(float time)
{
	if (keyFrames.find(time) == keyFrames.end())
	{
		times.push_back(time);
		keyFrames.insert(std::make_pair(time, KeyFrame(time)));
	}
	return &keyFrames[time];
}

Bone::Bone(aiNodeAnim* node, uint32_t id, Bone * parent) : name(node->mNodeName.C_Str()), id(id), parent(parent)
{
	for (uint32_t i = 0; i < node->mNumPositionKeys; ++i)
	{
		auto key = node->mPositionKeys[i];
		KeyFrame* keyFrame = GetOrCreateKeyFrame(key.mTime);
		keyFrame->type |= KeyFrame_Translate;
		keyFrame->translate = vec3(key.mValue);
	}
	for (uint32_t i = 0; i < node->mNumRotationKeys; ++i)
	{
		auto key = node->mRotationKeys[i];
		KeyFrame* keyFrame = GetOrCreateKeyFrame(key.mTime);
		keyFrame->type |= KeyFrame_Rotate;
		keyFrame->rotate = fquat(key.mValue);
	}
	for (uint32_t i = 0; i < node->mNumScalingKeys; ++i)
	{
		auto key = node->mScalingKeys[i];
		KeyFrame* keyFrame = GetOrCreateKeyFrame(key.mTime);
		keyFrame->type |= KeyFrame_Scale;
		keyFrame->scale = vec3(key.mValue);
	}

	//Fill in data that wasn't provided
	KeyFrame* last = nullptr;
	for (int32_t i=keyFrames.size()-1; i>=0; --i)
	{
		float time = times[i];
		auto kf = &keyFrames[time];
		if (kf->type != 7)
			printf("Missing Data!\n");

		kf->next = last;
		last = kf;
	}
}

inline uint32_t Bone::GetID()
{
	return id;
}

inline Bone * Bone::GetParent()
{
	return parent;
}

inline glm::mat4 Bone::GetOffset()
{
	return offset;
}

inline std::string Bone::GetName()
{
	return name;
}

float binarySearchNR(std::vector<float> arr, int l, int r, float x)
{
	while(r >= l)
	{
		int mid = l + (r - l) / 2;

		if (arr[mid] == x)
			return arr[mid];

		if (arr[mid] > x)
		{
			l = l;
			r = mid - 1;
			continue;
			//return binarySearch(arr, l, mid - 1, x);
		}

		l = mid + 1;
		r = r;

		//return binarySearch(arr, mid + 1, r, x);
	}

	return arr[r];
}

KeyFrame* Bone::GetPrevFrameToTime(float time)
{
	return &keyFrames[binarySearchNR(times, 0, times.size()-1, time)];
}

glm::mat4 Bone::GetMatrix(float time)
{
	KeyFrame* prev = GetPrevFrameToTime(time);
	if (prev == nullptr)
		return glm::mat4(1);
	KeyFrame* next = prev->next;
	if (next == nullptr)
		return prev->getMatrix();
	float ratio = (time - prev->time) / (next->time - prev->time);
	//printf("%lf\n", ratio);
	return prev->lerp(next, ratio).getMatrix();
	//return glm::rotate(glm::mat4(1), (float)time, glm::vec3(0, 1, 0));
}

glm::mat4 KeyFrame::getMatrix()
{
	glm::mat4 translate = glm::transpose(glm::translate(glm::mat4(1.0), this->translate));
	glm::mat4 rotate    = glm::transpose(glm::mat4_cast(this->rotate));
	glm::mat4 scale     = glm::transpose(glm::scale(glm::mat4(1.0), this->scale));
	glm::mat4 result = scale * rotate * translate;
	return result;
}

KeyFrame KeyFrame::lerp(KeyFrame * kf, float ratio)
{
	KeyFrame keyFrame;
	keyFrame.type = this->type;
	keyFrame.time = (time + (kf->time - time) * ratio);
	keyFrame.translate = (translate + (kf->translate - translate) * (float)ratio);
	keyFrame.rotate = glm::slerp(rotate, kf->rotate, (float)ratio);
	keyFrame.scale = (scale + (kf->scale - scale) * (float)ratio);
	return keyFrame;
}
