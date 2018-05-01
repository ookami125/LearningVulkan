#pragma once
#include <vector>
#include <glm\glm.hpp>

class Texture;
struct Vertex;
class Animation;

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
	uint32_t index_count;
	uint32_t* indices;
};

struct Model
{
	std::vector<Texture*> textures;
	std::vector<Material*> materials;
	std::vector<Mesh*> meshes;
	std::vector<Animation*> animations;

public:
	Model(std::string filepath);
};