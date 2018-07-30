#include "Animation.h"
#include "Utils.h"
#include <algorithm>
#include <stack>
#include "GLMAssimp.h"
#include "Model.h"
#include "Memory.h"
#include "MathUtils.h"

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
		//printf("checking %s...\n", name.c_str());
		auto find = std::find(animationNodeNames.begin(), animationNodeNames.end(), name);
		Bone* bone = nullptr;
		if (find != animationNodeNames.end())
		{
			uint32_t idx = std::distance(animationNodeNames.begin(), find);
			bone = aligned_new(Bone)(animationNodes[idx], idx, parentBone);
			bones.push_back(bone);
		}

		for (uint32_t i = 0; i < node->mNumChildren; ++i)
			nodesToVisit.push(std::make_pair(bone, node->mChildren[i]));
	}

	duration = (float)animation->mDuration;
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

//std::vector<std::pair<std::string, Mat4f>> Animation::GetAnimationFrame(float time)
//{
//	time *= tps;
//	if (time > duration) time = fmod(time, duration);
//	printf("time: %lf\n", time);
//	std::vector<std::pair<std::string, Mat4f>> mats(bones.size());
//	for (auto bone : bones)
//	{
//		Mat4f parentMat = (bone->GetParent()) ? mats[bone->GetParent()->GetID()].second : Mat4f(1);
//		mats[bone->GetID()] = std::make_pair(bone->GetName(), parentMat * bone->GetMatrix(time));
//	}
//	return mats;
//}

void Animation::GetAnimationFrame(std::vector<Mat4f*> boneMap, float time)
{
	time *= tps;
	if (time > duration) time = fmod(time, duration);
	//printf("time: %lf\n", time);
	for (auto bone : bones)
	{
		Mat4f parentMat = (bone->GetParent()) ? *boneMap[bone->GetParent()->GetID()] : Mat4f(1);
		*boneMap[bone->GetID()] = bone->GetMatrix(time) * parentMat;
	}
}

KeyFrame* Bone::GetOrCreateKeyFrame(float time)
{
	if (keyFrames.find(time) == keyFrames.end())
	{
		times.push_back(time);
		keyFrames.insert(std::make_pair(time, aligned_new(KeyFrame)(time)));
	}
	return keyFrames[time];
}

Bone::Bone(aiNodeAnim* node, uint32_t id, Bone * parent) : id(id), parent(parent)
{
	if (node == nullptr)
		return;
	name = node->mNodeName.C_Str();
	for (uint32_t i = 0; i < node->mNumPositionKeys; ++i)
	{
		auto key = node->mPositionKeys[i];
		KeyFrame* keyFrame = GetOrCreateKeyFrame((float)key.mTime);
		keyFrame->type |= KeyFrame_Translate;
		keyFrame->translate = vec3(key.mValue);
	}
	for (uint32_t i = 0; i < node->mNumRotationKeys; ++i)
	{
		auto key = node->mRotationKeys[i];
		KeyFrame* keyFrame = GetOrCreateKeyFrame((float)key.mTime);
		keyFrame->type |= KeyFrame_Rotate;
		keyFrame->rotate = fquat(key.mValue);
	}
	for (uint32_t i = 0; i < node->mNumScalingKeys; ++i)
	{
		auto key = node->mScalingKeys[i];
		KeyFrame* keyFrame = GetOrCreateKeyFrame((float)key.mTime);
		keyFrame->type |= KeyFrame_Scale;
		keyFrame->scale = vec3(key.mValue);
	}

	//Fill in data that wasn't provided
	KeyFrame* last = nullptr;
	for (int32_t i=keyFrames.size()-1; i>=0; --i)
	{
		float time = times[i];
		auto kf = keyFrames[time];
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

inline Mat4f Bone::GetOffset()
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
	return keyFrames[binarySearchNR(times, 0, times.size()-1, time)];
}

Mat4f Bone::GetMatrix(float time)
{
	KeyFrame* prev = GetPrevFrameToTime(time);
	if (prev == nullptr)
		return Mat4f(1);
	KeyFrame* next = prev->next;
	if (next == nullptr)
		return prev->getMatrix();
	float ratio = (time - prev->time) / (next->time - prev->time);
	//printf("%lf\n", ratio);
	return prev->lerp(next, ratio).getMatrix();
	//return glm::rotate(Mat4f(1), (float)time, glm::vec3(0, 1, 0));
}

Mat4f KeyFrame::getMatrix()
{
	Mat4f translate = MathUtils::translate(this->translate).Transpose();
	//Mat4f translate = glm::transpose(glm::translate(Mat4f(1.0), this->translate));
	Mat4f rotate = MathUtils::rotation(this->rotate).Transpose();
	//Mat4f rotate    = glm::transpose(Mat4f_cast(this->rotate));
	Mat4f scale = MathUtils::scale(this->scale).Transpose();
	//Mat4f scale     = glm::transpose(glm::scale(Mat4f(1.0), this->scale));
	Mat4f result = scale * rotate * translate;
	return result;
}

KeyFrame KeyFrame::lerp(KeyFrame * kf, float ratio)
{
	KeyFrame keyFrame;
	keyFrame.type = this->type;
	keyFrame.time = (time + (kf->time - time) * ratio);
	keyFrame.translate = (translate + (kf->translate - translate) * (float)ratio);
	//keyFrame.rotate = glm::slerp(rotate, kf->rotate, (float)ratio);
	keyFrame.scale = (scale + (kf->scale - scale) * (float)ratio);
	return keyFrame;
}
