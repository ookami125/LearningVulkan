#pragma once
#include <vector>
//#include <glm\glm.hpp>
#include "Math/vec4.h"
#include "Math/mat4.h"
#include <map>
#include <thread>
#include "Memory.h"

struct Texture;
struct Vertex;
class Animation;
class SkinnedMesh;

struct alignas(16) Material
{
	std::string name;
	Vec4f diffuseColor;
	std::vector<Texture*> diffuse;
	std::vector<Texture*> specular;
	std::vector<Texture*> ambient;
	std::vector<Texture*> emissive;
	std::vector<Texture*> height;
	std::vector<Texture*> normals;
	std::vector<Texture*> shininess;
	std::vector<Texture*> opacity;
	std::vector<Texture*> displacement;
	std::vector<Texture*> lightmap;
	std::vector<Texture*> reflection;
};

struct alignas(32) Mesh
{
	Material* mat;
	uint32_t vertices_count;
	Vertex* vertices;
	uint32_t indices_count;
	uint32_t* indices;
	std::vector<std::string> boneNames;
	std::vector<Mat4f, AlignmentAllocator<Mat4f>> boneOffsets;
	std::vector<Mat4f, AlignmentAllocator<Mat4f>> bones;
	std::vector<std::vector<Mat4f*>> boneMaps;
	Mat4f garbage;

	void* rendererData = nullptr;
};

struct alignas(32) Model
{
	Mat4f invTransform;
	std::vector<Texture*> textures;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;
	std::vector<Animation*> animations;
	void* rendererData;
public:
	Model(std::string filepath);

	std::vector<Mat4f, AlignmentAllocator<Mat4f>> GetAnimationFrame(int animationID, int meshID, double time);

	void LoadAnimations(std::string filepath);
};