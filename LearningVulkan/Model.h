#pragma once
#include <vector>
#include <glm\glm.hpp>
#include <map>

struct Texture;
struct Vertex;
class Animation;
class SkinnedMesh;

struct Material
{
	std::string name;
	glm::vec3 diffuseColor;
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

struct Mesh
{
	Material* mat;
	uint32_t vertices_count;
	Vertex* vertices;
	uint32_t indices_count;
	uint32_t* indices;
	std::vector<std::string> boneNames;
	std::vector<glm::mat4> boneOffsets;
	std::vector<glm::mat4> bones;
	std::vector<std::vector<glm::mat4*>> boneMaps;
	glm::mat4 garbage;

	void* rendererData = nullptr;
};

struct Model
{
	std::vector<Texture*> textures;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;
	std::vector<Animation*> animations;
	glm::mat4 invTransform;
	void* rendererData;

public:
	Model(std::string filepath);

	std::vector<glm::mat4> GetAnimationFrame(int animationID, int meshID, double time);

	void LoadAnimations(std::string filepath);
};