#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\component_wise.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GLMAssimp.h"
#include "Vertex.h"
#include "ResourceManager.h"
#include "Animation.h"

Model::Model(std::string filepath)
{
	Assimp::Importer importer;
	auto scene = importer.ReadFile(filepath.c_str(), aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_Triangulate | aiProcess_FlipWindingOrder);
	if (!scene) {
		throw std::runtime_error(importer.GetErrorString());
	}

	if (scene->HasTextures())
	{
		int numTextures = scene->mNumTextures;
		for (int i = 0; i < numTextures; i++)
		{
			auto texture = scene->mTextures[i];
			auto tex = ResourceManager::LoadTexture(texture);
			textures.push_back(tex);
			printf("texture format: %s\n", texture->achFormatHint);
		}
	}

	if (scene->HasMaterials())
	{
		int numMaterials = scene->mNumMaterials;
		for (int i = 0; i < numMaterials; ++i)
		{
			Material* mat = new Material();
			auto material = scene->mMaterials[i];

			aiString name;
			material->Get(AI_MATKEY_NAME, name);
			printf("AI_MATKEY_NAME: %s\n", name.C_Str());
			mat->name = name.C_Str();

			aiColor3D color(0.f, 0.f, 0.f);
			material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			printf("AI_MATKEY_COLOR_DIFFUSE: vec3(%f, %f, %f)\n", color.r, color.g, color.b);
			mat->diffuseColor = vec3(color);

			for (aiTextureType TextureType = aiTextureType_DIFFUSE; TextureType < aiTextureType_UNKNOWN; TextureType = (aiTextureType)((int)TextureType + 1))
			{
				int textureCount = material->GetTextureCount(TextureType);
				//if (textureCount > 0)
				//	printf("%s\n", aiTextureTypeName(TextureType).c_str());
				for (int j = 0; j < textureCount; ++j)
				{
					aiString temp;
					material->GetTexture(TextureType, j, &temp);
					std::string path = temp.C_Str();
					Texture* tex = nullptr;
					if (path.find('*') != -1)
					{
						int idx = atoi(path.c_str() + 1);
						tex = textures[idx];
					}
					else
					{
						printf("TODO: External image loader.");
					}

					switch (TextureType)
					{
					case aiTextureType_DIFFUSE:
						mat->diffuse.push_back(tex);
						break;
					case aiTextureType_SPECULAR:
						mat->specular.push_back(tex);
						break;
					case aiTextureType_AMBIENT:
						mat->ambient.push_back(tex);
						break;
					case aiTextureType_EMISSIVE:
						mat->emissive.push_back(tex);
						break;
					case aiTextureType_HEIGHT:
						mat->height.push_back(tex);
						break;
					case aiTextureType_NORMALS:
						mat->normals.push_back(tex);
						break;
					case aiTextureType_SHININESS:
						mat->shininess.push_back(tex);
						break;
					case aiTextureType_OPACITY:
						mat->opacity.push_back(tex);
						break;
					case aiTextureType_DISPLACEMENT:
						mat->displacement.push_back(tex);
						break;
					case aiTextureType_LIGHTMAP:
						mat->lightmap.push_back(tex);
						break;
					case aiTextureType_REFLECTION:
						mat->reflection.push_back(tex);
						break;
					default:
						break;
					}
				}
			}
			materials.push_back(mat);
		}
	}

	if (scene->HasAnimations())
	{
		printf("Animations:\n");
		int numAnimations = scene->mNumAnimations;
		for (int i = 0; i < numAnimations; i++)
		{
			auto animation = scene->mAnimations[i];
			Animation* anim = new Animation(animation, scene);
			//anim->SetDuration(animation->mDuration);
			//printf("  %s\n", name.C_Str());
			//
			//int numChannels = animation->mNumChannels;
			//for (int j = 0; j < numChannels; ++j)
			//{
			//	auto channel = animation->mChannels[j];
			//	Bone* bone = new Bone(channel->mNodeName.C_Str());
			//	//printf("    node name: %s\n", channel->mNodeName.C_Str());
			//
			//	int numPositionKeys = channel->mNumPositionKeys;
			//	for (int k = 0; k < numPositionKeys; ++k)
			//	{
			//		auto positionKey = channel->mPositionKeys[k];
			//		glm::vec3 pos = vec3(positionKey.mValue);
			//		bone->AddPositionKey(positionKey.mTime, pos);
			//		//printf("      %f - {%f,%f,%f}\n", positionKey.mTime, pos.x, pos.y, pos.z);
			//	}
			//
			//	int numRotationKeys = channel->mNumRotationKeys;
			//	for (int k = 0; k < numRotationKeys; ++k)
			//	{
			//		auto rotationKey = channel->mRotationKeys[k];
			//		glm::fquat rot = fquat(rotationKey.mValue);
			//		bone->AddRotationKey(rotationKey.mTime, rot);
			//		//printf("      %f - {%f,%f,%f,%f}\n", rotationKey.mTime, rot.x, rot.y, rot.z, rot.w);
			//	}
			//
			//	int numScalingKeys = channel->mNumScalingKeys;
			//	for (int k = 0; k < numScalingKeys; ++k)
			//	{
			//		auto scalingKey = channel->mScalingKeys[k];
			//		glm::vec3 scale = vec3(scalingKey.mValue);
			//		bone->AddScalingKey(scalingKey.mTime, scale);
			//		//printf("      %f - {%f,%f,%f}\n", scalingKey.mTime, scale.x, scale.y, scale.z);
			//	}
			//
			//	aiString parentBone = scene->mRootNode->FindNode(channel->mNodeName)->mParent->mName;
			//	anim->AddBone(bone, parentBone.C_Str());
			//}
			//
			//anim->Compile();
			animations.push_back(anim);
		}
	}

	if (scene->HasMeshes())
	{
		int numMeshes = scene->mNumMeshes;
		for (int i = 0; i < numMeshes; i++)
		{
			auto mesh = scene->mMeshes[i];
			auto materialIdx = mesh->mMaterialIndex;
			Mesh* meshP = new Mesh();
			meshP->mat = materials[materialIdx];

			if (mesh->HasFaces())
			{
				std::vector<uint32_t> indices;
				int numFaces = mesh->mNumFaces;
				for (int j = 0; j < numFaces; j++)
				{
					auto face = mesh->mFaces[j];
					indices.push_back(face.mIndices[0]);
					indices.push_back(face.mIndices[1]);
					indices.push_back(face.mIndices[2]);
				}
				meshP->indices = (uint32_t*)malloc(sizeof(uint32_t) * indices.size());
				memcpy(meshP->indices, &indices[0], sizeof(uint32_t) * indices.size());
				meshP->indices_count = indices.size();
			}

			if (mesh->HasPositions())
			{
				std::vector<Vertex> vertices;
				int numPositions = mesh->mNumVertices;
				for (int j = 0; j < numPositions; j++)
				{
					Vertex Vert;
					Vert.pos = vec3(mesh->mVertices[j]);
					Vert.color = (mesh->HasVertexColors(0)) ? (glm::vec3)vec4(mesh->mColors[0][j]) : glm::vec3(1);
					Vert.texCoord = (mesh->HasTextureCoords(0)) ? (glm::vec2)vec3(mesh->mTextureCoords[0][j]) : glm::vec2{ 0, 0 };
					Vert.bonesIdx = glm::uvec4(0);
					Vert.bonesWeights = glm::vec4(0);
					vertices.push_back(Vert);
				}
				meshP->vertices = (Vertex*)malloc(sizeof(Vertex) * vertices.size());
				memcpy(meshP->vertices, &vertices[0], sizeof(Vertex) * vertices.size());
				meshP->vertices_count = vertices.size();
			}

			if (mesh->HasBones())
			{
				meshP->boneOffsets = (glm::mat4*)malloc(sizeof(glm::mat4) * mesh->mNumBones);
				for (uint32_t j = 0; j < mesh->mNumBones; ++j)
				{
					auto bone = mesh->mBones[j];

					meshP->boneOffsets[j] = glm::make_mat4(&bone->mOffsetMatrix.a1);
					for (uint32_t k = 0; k < bone->mNumWeights; ++k)
					{
						auto weight = bone->mWeights[k];

						uint32_t boneIdx = j;
						float boneWeight = weight.mWeight;
						//printf("%d\n", weight.mVertexId);
						Vertex& vertex = meshP->vertices[weight.mVertexId];
						for (int l = 0; l < 4; ++l)
						{
							if (vertex.bonesWeights[l] < weight.mWeight)
							{
								std::swap(vertex.bonesIdx[l], boneIdx);
								std::swap(vertex.bonesWeights[l], boneWeight);
							}
						}
					}
				}

				for (uint32_t j = 0; j < meshP->vertices_count; ++j)
				{
					float total = glm::compAdd(meshP->vertices[j].bonesWeights);
					meshP->vertices[j].bonesWeights *= glm::vec4(1.0f / total);
				}
			}
			meshes.push_back(meshP);
		}
	}
	invTransform = glm::inverse(glm::make_mat4(&scene->mRootNode->mTransformation.a1));
}

std::vector<glm::mat4> Model::GetAnimationFrame(int animationID, int meshID, double time)
{
	std::vector<glm::mat4> mats = animations[animationID]->GetAnimationFrame(time);
	for (uint32_t i = 0; i < mats.size(); ++i)
		mats[i] = invTransform * mats[i] * meshes[meshID]->boneOffsets[i];
	return mats;
}
