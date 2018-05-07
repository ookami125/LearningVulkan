#include "Animation.h"
#include "Utils.h"
#include <algorithm>
#include <queue>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\matrix_interpolation.hpp>
#include "GLMAssimp.h"

Animation::Animation(const aiAnimation * animation, const aiScene * scene)
{
	std::unordered_set<std::string> nodeNames;
	for(uint32_t i=0; i<animation->mNumChannels; ++i)
		nodeNames.insert(animation->mChannels[i]->mNodeName.C_Str());
	aiNode* rootNode = nullptr;
	for (std::string nodeName : nodeNames)
	{
		auto temp = scene->mRootNode->FindNode(nodeName.c_str());
		std::string parentName = temp->mParent->mName.C_Str();
		if (nodeNames.find(parentName) == nodeNames.end())
		{
			rootNode = temp;
			break;
		}
	}
	printf("root node: %s\n", rootNode->mName.C_Str());
	std::map<aiNode*, aiNodeAnim*> node2animNode;
	for (uint32_t i = 0; i < animation->mNumChannels; ++i)
	{
		auto animNode = animation->mChannels[i];
		auto node = scene->mRootNode->FindNode(animNode->mNodeName.C_Str());
		node2animNode.insert(std::make_pair(node, animNode));
	}

	std::queue<std::pair<aiNode*, aiNode*>> toVisit;
	toVisit.push(std::make_pair(nullptr, rootNode));

	//Bone* parent = nullptr;
	std::map<aiNode*, Bone*> node2bone;
	node2bone.insert(std::pair<aiNode*, Bone*>(nullptr, nullptr));
	while (toVisit.size() > 0)
	{
		aiNode* parentNode = toVisit.front().first;
		aiNode* node = toVisit.front().second;
		toVisit.pop();
		for (uint32_t i = 0; i < node->mNumChildren; ++i)
			toVisit.push(std::make_pair(node, node->mChildren[i]));
		aiNodeAnim* aNode = node2animNode[node];
		if (aNode == nullptr)
			continue;
		uint32_t id = std::distance(nodeNames.begin(), nodeNames.find(node->mName.C_Str()));
		Bone* bone = new Bone(aNode, id, node2bone[parentNode]);
		bones.push_back(bone);
		node2bone.insert(std::make_pair(node, bone));
	}

	printf("stage2\n");
}

std::vector<glm::mat4> Animation::GetAnimationFrame(double time)
{
	std::vector<glm::mat4> mats(bones.size());
	for (auto bone : bones)
	{
		mats[bone->GetID()] = (bone->GetParent()) ? mats[bone->GetParent()->GetID()] : glm::mat4(1);
		mats[bone->GetID()] *= bone->GetMatrix(time);
	}
	return mats;
}

KeyFrame* Bone::GetOrCreateKeyFrame(double time)
{
	if (keyFrames.find(time) == keyFrames.end())
		keyFrames.insert(std::make_pair(time, KeyFrame()));
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
	//for()
}

inline uint32_t Bone::GetID()
{
	return id;
}

inline Bone * Bone::GetParent()
{
	return parent;
}

glm::mat4 Bone::GetMatrix(double time)
{
	return glm::rotate(glm::mat4(1), (float)time, glm::vec3(0, 1, 0));
}

glm::mat4 KeyFrame::getMatrix()
{
	glm::mat4 ModelMatrix = glm::mat4();
	glm::mat4 translate = glm::translate(glm::mat4(1.0), this->translate);
	glm::mat4 rotate = glm::mat4_cast(this->rotate);
	glm::mat4 scale = glm::scale(glm::mat4(1.0), this->scale);
	return translate * rotate * scale;
}
