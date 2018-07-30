#pragma once
#include <map>
#include <vector>
#include <set>
#include <unordered_set>
//#include <glm\glm.hpp>
//#include <glm\gtc\quaternion.hpp>
#include "Math/vec4.h"
#include "Math/mat4.h"
#include "Math/quat.h"
#include <assimp\anim.h>
#include <assimp\scene.h>

#define MAX_BONE_COUNT 62

struct Mesh;

enum KeyFrameBitTypes
{
	KeyFrame_Translate = 1,
	KeyFrame_Rotate = 2,
	KeyFrame_Scale = 4,
};

struct alignas(16) KeyFrame
{
	Vec4f translate = Vec4f(0);
	Quatf rotate = Quatf();
	Vec4f scale = Vec4f(1);
	uint8_t type = 0;
	KeyFrame* next = nullptr;
	float time;
	Mat4f getMatrix();

	KeyFrame() = default;
	KeyFrame(float time) : time(time) {};

	KeyFrame lerp(KeyFrame* kf, float ratio);
};

class alignas(32) Bone
{
	std::string name;
	uint32_t id;
	Bone* parent = nullptr;
	Mat4f offset;
	std::vector<float> times;
	std::map<float, KeyFrame*> keyFrames;
	KeyFrame * GetOrCreateKeyFrame(float time);
public:
	Bone(aiNodeAnim* node, uint32_t id, Bone* parent);

	inline uint32_t GetID();
	inline Bone* GetParent();
	inline Mat4f GetOffset();
	inline std::string GetName();

	KeyFrame * GetPrevFrameToTime(float time);

	Mat4f GetMatrix(float time);
};

class Animation
{
	float duration = 0.0f;
	float tps; //ticks per second
	std::vector<std::string> animationNodeNames;
	std::vector<Bone*> bones;
public:
	Animation(const aiAnimation* animation, const aiScene* scene);

	std::vector<std::string> GetLayout();

	std::string GetBoneName(int i);

	void GetAnimationFrame(std::vector<Mat4f*> boneMap, float time);
};