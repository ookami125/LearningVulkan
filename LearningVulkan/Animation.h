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
	uint8_t type = 0;
	KeyFrame* next = nullptr;
	double time;
	glm::vec3 translate = glm::vec3(0);
	glm::fquat rotate = glm::fquat();
	glm::vec3 scale = glm::vec3(1);
	glm::mat4 getMatrix();

	KeyFrame() = default;
	KeyFrame(double time) : time(time) {};

	KeyFrame lerp(KeyFrame* kf, double ratio);
};

class Bone
{
	std::string name;
	uint32_t id;
	Bone* parent = nullptr;
	glm::mat4 offset;
	std::vector<double> times;
	std::map<double, KeyFrame> keyFrames;
	KeyFrame * GetOrCreateKeyFrame(double time);
public:
	Bone(aiNodeAnim* node, uint32_t id, Bone* parent);

	inline uint32_t GetID();
	inline Bone* GetParent();
	inline glm::mat4 GetOffset();
	inline std::string GetName();

	KeyFrame * GetPrevFrameToTime(double time);

	glm::mat4 GetMatrix(double time);
};

class Animation
{
	double duration = 0.0f;
	double tps; //ticks per second
	std::vector<std::string> animationNodeNames;
	std::vector<Bone*> bones;
public:
	Animation(const aiAnimation* animation, const aiScene* scene);

	std::string GetBoneName(int i);

	std::vector<std::pair<std::string, glm::mat4>> GetAnimationFrame(double time);
};