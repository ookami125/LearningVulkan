#include "Animation.h"
#include "Utils.h"
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\matrix_interpolation.hpp>

Frame * Bone::GetOrNewFrame(double time)
{
	Frame* frame;
	if (std::binary_search(times.begin(), times.end(), time)) {
		frame = frames[time];
	} else {
		frame = new Frame();
		frame->time = time;
		frames.insert(std::make_pair(time, frame));
		times.push_back(time);
		std::sort(times.begin(), times.end());
	}
	return frame;
}

double Bone::GetDuration()
{
	return parentAnimation->GetDuration();
}

Bone::Bone(std::string name) : name(name) {}

void Bone::SetParentAnimation(Animation * animation)
{
	parentAnimation = animation;
}

void Bone::AddPositionKey(double time, glm::vec3 position)
{
	Frame* frame = GetOrNewFrame(time);
	frame->frameData = (FrameData)(frame->frameData | PositionFrame);
	frame->position = position;
}

void Bone::AddRotationKey(double time, glm::fquat rotation)
{
	Frame* frame = GetOrNewFrame(time);
	frame->frameData = (FrameData)(frame->frameData | RotationFrame);
	frame->rotation = rotation;
}

void Bone::AddScalingKey(double time, glm::vec3 scale)
{
	Frame* frame = GetOrNewFrame(time);
	frame->frameData = (FrameData)(frame->frameData | ScalingFrame);
	frame->scale = scale;
}

std::string Bone::GetName()
{
	return name;
}

void Bone::Compile()
{
	glm::vec3 lastPosition;
	glm::quat lastRotation;
	glm::vec3 lastScaling;
	Frame* prev = nullptr;
	for (double time : times)
	{
		Frame* frame = frames[time];
		frame->prev = prev;
		if (prev)
			prev->next = frame;
		else
			firstFrame = frame;
		
		if (frame->frameData & PositionFrame)
			lastPosition = frame->position;
		else
			frame->position = lastPosition;

		if (frame->frameData & RotationFrame)
			lastRotation = frame->rotation;
		else
			frame->rotation = lastRotation;

		if (frame->frameData & ScalingFrame)
			lastScaling = frame->scale;
		else
			frame->scale = lastScaling;

		glm::mat4 translate = glm::translate(glm::mat4(1.0), frame->position);
		glm::mat4 rotate = glm::mat4_cast(frame->rotation);
		glm::mat4 scale = glm::scale(glm::mat4(1.0), frame->scale);
		frame->matrix = translate * rotate * scale;

		prev = frame;
	}
	lastFrame = prev;

	//TODO: remove useless frames
	//for (double time : times)
	//{
	//	
	//}
}

Frame * Bone::GetFrameBeforeTime(double time)
{
	int l = 0;
	int r = times.size();
	if (time > GetDuration() || time < 0.0)
		return frames[times[times.size() - 1]];
	while (l <= r)
	{
		int m = l + (r - l) / 2;

		if (times[m] == time)
			return frames[time];

		if (times[m] < time)
			l = m + 1;
		else
			r = m - 1;
	}
	return frames[times[r]];
}

glm::mat4 Bone::GetInterpolatedFrameMatrix(double time)
{
	Frame frame;
	Frame* prevFrame = GetFrameBeforeTime(time);
	if (prevFrame->time == time)
		return prevFrame->matrix;

	float delta = 0.0;
	Frame* nextFrame = prevFrame->next;

	if (!nextFrame){
		nextFrame = firstFrame;
		delta = (float)(((float)(time - prevFrame->time)) / (GetDuration() - prevFrame->time));
	}else{
		delta = (float)(((float)(time - prevFrame->time)) / (nextFrame->time - prevFrame->time));
	}

	glm::mat4 ret = glm::interpolate(prevFrame->matrix, nextFrame->matrix, delta);
	return ret;
}

glm::mat4 Bone::GetInterpolatedFrameMatrixGlobal(double time)
{
	return glm::mat4();
}

Animation::Animation(std::string name) : name(name) {}

void Animation::SetDuration(double time)
{
	duration = time;
}

void Animation::AddBone(Bone * bone, int parentIdx)
{
	bone->SetParentAnimation(this);
	bones.push_back(bone);
	boneNames.insert(bone->GetName());
	boneParent.push_back(parentIdx);
}

double Animation::GetDuration()
{
	return duration;
}

void Animation::Compile()
{
	for (Bone* bone : bones)
		bone->Compile();
}

std::vector<glm::mat4> Animation::GetAnimationFrame(double time)
{
	if (time > GetDuration()) //Don't let time go over duration
		time = fmod(time, GetDuration());
	std::vector<glm::mat4> mats(bones.size());
	for (int i=0; i<bones.size(); ++i)
		mats[i] = bones[i]->GetInterpolatedFrameMatrix(time);
	return mats;
}
