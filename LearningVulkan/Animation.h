#pragma once
#include <map>
#include <vector>
#include <set>
#include <unordered_set>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <assimp\anim.h>
#include <assimp\scene.h>

enum KeyFrameBitTypes
{
	KeyFrame_Translate = 1,
	KeyFrame_Rotate = 2,
	KeyFrame_Scale = 4,
};

struct KeyFrame
{
	uint8_t type;
	glm::vec3 translate;
	glm::fquat rotate;
	glm::vec3 scale;
	glm::mat4 getMatrix();
};

class Bone
{
	std::string name;
	uint32_t id;
	Bone* parent = nullptr;
	std::map<double, KeyFrame> keyFrames;
	KeyFrame * GetOrCreateKeyFrame(double time);
public:
	Bone(aiNodeAnim* node, uint32_t id, Bone* parent);

	inline uint32_t GetID();
	inline Bone* GetParent();

	glm::mat4 GetMatrix(double time);
};

class Animation
{
	std::vector<Bone*> bones;
public:
	Animation(const aiAnimation* animation, const aiScene* scene);

	std::vector<glm::mat4> GetAnimationFrame(double time);
};