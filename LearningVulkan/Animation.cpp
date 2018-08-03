#include "Animation.h"
#include "Utils.h"
#include <algorithm>
#include <stack>
#include "GLMAssimp.h"
#include "Model.h"
#include "Memory.h"
#include "Math/MathUtils.h"

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

static void printMat(const char* name, Mat4f m)
{
	printf("\n%s:\n", name);
	float d[16];
	m.row[0].Store(d);
	m.row[1].Store(d + 4);
	m.row[2].Store(d + 8);
	m.row[3].Store(d + 12);
	printf("%f %f %f %f\n", d[0], d[1], d[2], d[3]);
	printf("%f %f %f %f\n", d[4], d[5], d[6], d[7]);
	printf("%f %f %f %f\n", d[8], d[9], d[10], d[11]);
	printf("%f %f %f %f\n", d[12], d[13], d[14], d[15]);
}

void Animation::GetAnimationFrame(std::vector<Mat4f*> boneMap, float time)
{
	time *= tps;
	if (time > duration) time = fmod(time, duration);
	//printf("time: %lf\n", time);
	for (auto bone : bones)
	{
		auto parent = bone->GetParent();
		Mat4f parentMat = parent ? *boneMap[parent->GetID()] : Mat4f(1);
		Mat4f boneMat = bone->GetMatrix(time);
		*boneMap[bone->GetID()] = parentMat * boneMat;
		//printMat(bone->GetName().c_str(), boneMat);
	}
	return;
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
		keyFrame->translate = vec3(key.mValue, 1);
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
		keyFrame->scale = vec3(key.mValue, 0);
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

//#define GLM_FORCE_SWIZZLE
//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/quaternion.hpp>

Mat4f KeyFrame::getMatrix()
{
	//glm::vec4 thisTranslate; this->translate.Store(&thisTranslate.x, &thisTranslate.y, &thisTranslate.z, &thisTranslate.w);
	//glm::mat4 translate2 = glm::transpose(glm::translate(glm::mat4(1.0), glm::vec3(thisTranslate.xyz)));
	//glm::quat thisRotate; this->rotate.Store(&thisRotate.w, &thisRotate.x, &thisRotate.y, &thisRotate.z);
	//glm::mat4 rotate2    = glm::transpose(glm::toMat4(thisRotate));
	//glm::vec4 thisScale; this->scale.Store(&thisScale.x, &thisScale.y, &thisScale.z, &thisScale.w);
	//glm::mat4 scale2     = glm::transpose(glm::scale(glm::mat4(1.0), glm::vec3(thisScale.xyz)));
	//glm::mat4 result2 = scale2 * rotate2 * translate2;

	Mat4f translate = MathUtils::translate(this->translate);
	//printMat("translate", translate);
	Mat4f rotate = MathUtils::rotation(this->rotate);// .Transpose();
	//printMat("rotate", rotate);
	Mat4f scale = MathUtils::scale(this->scale);
	//printMat("scale", scale);
	Mat4f result = translate  * rotate * scale;
	//printMat("result", result);
	return result;
}

KeyFrame KeyFrame::lerp(KeyFrame * kf, float ratio)
{
	KeyFrame keyFrame;
	keyFrame.type = this->type;
	keyFrame.time = (time + (kf->time - time) * ratio);
	keyFrame.translate = (translate + (kf->translate - translate) * (float)ratio);
	//glm::vec4 rot = glm::slerp(glm::fquat(), glm::fquat(), 0.0f);
	keyFrame.rotate = MathUtils::slerp(rotate, kf->rotate, (float)ratio);
	keyFrame.scale = (scale + (kf->scale - scale) * (float)ratio);
	return keyFrame;
}
