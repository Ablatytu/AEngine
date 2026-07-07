GLSLPROGRAM
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

#include "Shaders/Core.glsl"
uniform mat4 transform;
out vec2 TexCoord;
out vec3 Normal;

void main()
{
	gl_Position = proj * view * transform * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
	Normal = mat3(transpose(inverse(transform))) * aNormal;
}