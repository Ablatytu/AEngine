GLSLPROGRAM
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

layout (location = 2) in vec4 Position;
layout (location = 3) in vec4 Color;
layout (location = 4) in float Rotation;

#include "Shaders/Core.glsl"

out vec2 TexCoords;
out vec4 ColorParticle;



void main()
{
	TexCoords = aTexCoords;
	ColorParticle = Color;

	float xrot = cos(Rotation);
	float yrot = sin(Rotation);

	vec2 localPos = vec2(aPos.x * xrot - aPos.y * yrot, aPos.x * yrot + aPos.y * xrot);

	localPos *= Position.w;

	vec4 viewSpace = view * vec4(Position.xyz, 1.0);

	viewSpace.xy += localPos;

	gl_Position = proj * viewSpace;
}