#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UBO_ViewProj {
    mat4 view;
    mat4 proj;
} viewProj;

//layout(set = 1, binding = 0) uniform UBO_Model {
//    mat4 model;
//    mat4 bones[100];
//} model;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in uvec4 inBoneIdx;
layout(location = 4) in vec4 inBoneWeight;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

out gl_PerVertex {
    vec4 gl_Position;
};

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    gl_Position = viewProj.proj * viewProj.view * mat4(1) * vec4(inPosition, 1.0);
    vec3 color1 = hsv2rgb(vec3(float(inBoneIdx.x)/100.0, 1.0, 1.0)) * inBoneWeight.x;
    vec3 color2 = hsv2rgb(vec3(float(inBoneIdx.y)/100.0, 1.0, 1.0)) * inBoneWeight.y;
    vec3 color3 = hsv2rgb(vec3(float(inBoneIdx.z)/100.0, 1.0, 1.0)) * inBoneWeight.z;
    vec3 color4 = hsv2rgb(vec3(float(inBoneIdx.w)/100.0, 1.0, 1.0)) * inBoneWeight.w;
    fragColor = color1+color2+color3+color4;
    fragTexCoord = inTexCoord;
}