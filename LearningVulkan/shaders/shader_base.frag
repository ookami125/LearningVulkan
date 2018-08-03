#version 450
#extension GL_ARB_separate_shader_objects : enable

//layout(binding = 2) uniform sampler2DArray texSampler;
//layout(binding = 3) uniform UBOModelTextureId
//{
//	int id;
//} texID;

layout(binding = 2) uniform sampler samp;
layout(binding = 3) uniform texture2D textures[8];

layout(binding = 4) uniform TextureData {
    int DiffuseTextureID;
} inTextureData;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragWorldPos;
layout(location = 3) flat in uint TextureID;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(sampler2D(textures[/*inTextureData.DiffuseTextureID*/0], samp), fragTexCoord);
}