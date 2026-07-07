GLSLPROGRAM
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;

#include "Shaders/Core.glsl"

uniform mat4 transform;

void main()
{
	gl_Position = proj * view * transform * vec4(aPos + aNormal * 0.02, 1.0f);
}