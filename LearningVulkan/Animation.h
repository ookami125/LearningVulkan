#pragma once
#include <map>
#include <vector>
#include <unordered_set>
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>

enum FrameData
{
	PositionFrame = 1,
	RotationFrame = 2,
	ScalingFrame = 4,
};

struct Frame
{
	double time;
	Frame* next = nullptr;
	Frame* prev = nullptr;

	FrameData frameData;

	glm::vec3 position;
	glm::fquat rotation;
	glm::vec3 scale;

	glm::mat4 matrix;
};

class Animation;

class Bone
{
	std::string name;
	std::map<double, Frame*> frames;
	std::vector<double> times;
	Animation* parentAnimation;

	//After Compile Varialbles
	Frame* firstFrame;
	Frame* lastFrame;

private:
	Frame* GetOrNewFrame(double time);
	double GetDuration();

public:
	Bone(std::string name);

	void SetParentAnimation(Animation* animation);

	void AddPositionKey(double time, glm::vec3 position);
	void AddRotationKey(double time, glm::fquat rotation);
	void AddScalingKey(double time, glm::vec3 scale);

	std::string GetName();

	void Compile();

	//After Compile Functions
	Frame* GetFrameBeforeTime(double time);
	glm::mat4 GetInterpolatedFrameMatrix(double time);
	glm::mat4 GetInterpolatedFrameMatrixGlobal(double time);
};

class Animation
{
	std::string name;
	std::unordered_set<std::string> boneNames;
	std::vector<Bone*> bones;
	std::vector<int> boneParent;
	double duration;

public:
	Animation(std::string name);

	void SetDuration(double time);
	void AddBone(Bone* bone, int parentIdx);
	

	double GetDuration();

	void Compile();

	std::vector<glm::mat4> GetAnimationFrame(double time);
};