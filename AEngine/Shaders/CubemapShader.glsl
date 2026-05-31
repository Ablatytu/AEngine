Properties
{
	cubemap = (TextureCube, white)
}
GLSLPROGRAM
#version 330 core

#include "Shaders/Core.glsl"

in VS_OUT 
{
 vec2 texCoords;
 vec3 Normal;
 vec3 FragPos;
 float time;
 vec4 FragPosLightSpace;
 mat3 TBN;
 vec3 TangentViewDir;
} fs_in;

out vec4 FragColor;

uniform samplerCube cubemap;

void main()
{
	float ratio = 1.00 / 1.33;
	vec3 I = normalize(fs_in.FragPos - camPos);
	vec3 R = refract(I, normalize(fs_in.Normal), ratio);
	FragColor = vec4(texture(cubemap, R).rgb, 1.0);
}