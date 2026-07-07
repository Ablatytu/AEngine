GLSLPROGRAM
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

#include "Shaders/Core.glsl"

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
uniform mat4 transform;

void main()
{
	gl_Position = proj * view * transform * vec4(aPos, 1.0);
	FragPos = vec3(transform * vec4(aPos, 1.0));
	Normal =  mat3(transpose(inverse(transform))) * aNormal;
	TexCoords = aTexCoord;
}