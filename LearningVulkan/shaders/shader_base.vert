#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform ViewProj {
    mat4 view;
    mat4 proj;
} inViewProj;

layout(binding = 1) uniform ModelData {
    mat4 model;
	mat4 bones[62];
	uint textureId;
} inModelData;


layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec4 inTexCoord;
layout(location = 3) in uvec4 inBoneIdx;
layout(location = 4) in vec4 inBoneWeight;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragWorldPos;
layout(location = 3) out uint TextureID;

out gl_PerVertex {
    vec4 gl_Position;
};

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    
	mat4 boneTransform = inModelData.bones[inBoneIdx.x] * inBoneWeight.x;
	boneTransform     += inModelData.bones[inBoneIdx.y] * inBoneWeight.y;
	boneTransform     += inModelData.bones[inBoneIdx.z] * inBoneWeight.z;
	boneTransform     += inModelData.bones[inBoneIdx.w] * inBoneWeight.w;	
	
    //mat4 model = mat4(
    //    vec4(0.1, 0.0, 0.0, 0.0), 
    //    vec4(0.0, 0.1, 0.0, 0.0), 
    //    vec4(0.0, 0.0, 0.1, 0.0), 
    //    vec4(0.0, 0.0, 0.0, 1.0));

	gl_Position = inViewProj.proj * inViewProj.view * inModelData.model * boneTransform * inPosition;

    vec3 color1 = hsv2rgb(vec3(float(inBoneIdx.x)/5.0, 1.0, 1.0)) * inBoneWeight.x;
    vec3 color2 = hsv2rgb(vec3(float(inBoneIdx.y)/5.0, 1.0, 1.0)) * inBoneWeight.y;
    vec3 color3 = hsv2rgb(vec3(float(inBoneIdx.z)/5.0, 1.0, 1.0)) * inBoneWeight.z;
    vec3 color4 = hsv2rgb(vec3(float(inBoneIdx.w)/5.0, 1.0, 1.0)) * inBoneWeight.w;
    fragColor = color1+color2+color3+color4;
    fragTexCoord = inTexCoord.xy;
}