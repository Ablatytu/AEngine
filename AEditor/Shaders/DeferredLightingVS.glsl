GLSLPROGRAM
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in mat4 instanceMatrix;

#include "Shaders/Core.glsl"

flat out int LightIndex;

void main()
{
    LightIndex = gl_InstanceID;
    gl_Position = proj * view * instanceMatrix * vec4(aPos, 1.0);
}

