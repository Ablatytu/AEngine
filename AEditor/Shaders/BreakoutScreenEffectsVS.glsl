GLSLPROGRAM
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

#include "Shaders/Core.glsl"

out vec2 TexCoord;

uniform bool chaos;
uniform bool confuse;
uniform bool shake;

void main()
{
	gl_Position = vec4(aPos.xy, 0.0f, 1.0f);
	if(chaos)
	{
		float strength = 0.3;
		vec2 pos = vec2(aTexCoord.x + sin(_Time) * strength, aTexCoord.y + cos(_Time) * strength);
		TexCoord = pos;
	}
	else if(confuse)
	{
		TexCoord = vec2(1.0 - aTexCoord.x, 1.0 - aTexCoord.y);
	}
	else
	{
		TexCoord = aTexCoord;
	}
	if(shake)
	{
		vec2 shakeEffect = vec2(cos(_Time * 10), cos(_Time * 15));
		gl_Position = vec4(aPos + (shakeEffect * 0.01), 0.0, 1.0);
	}
}