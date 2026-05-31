Properties 
{
	_MainTexture = (Texture2D, white)
	_Color = (Color, (1.0, 1.0, 1.0, 1.0))
	[Range(0.0, 1.0)] _Threshold = (Float, 0.5)
}

RenderPath "Forward"
GLSLPROGRAM
#version 330 core
#include "Shaders/Lighting.glsl"

uniform sampler2D _MainTexture;
uniform vec4 _Color;
uniform float _Threshold;

in vec2 TexCoord;
in vec3 Normal;
out vec4 FragColor;

void main()
{
	float toon = 0.3;

	if(countDirLight > 0)
	{
		vec3 N = normalize(Normal);
		vec3 L = normalize(-DirectionalLights[0].direction);

		float ndotL = max(dot(N, L), 0.0);

		toon = ndotL > _Threshold ? 1.0 : 0.3;
	}

	FragColor = vec4(vec3(texture(_MainTexture, TexCoord)) * vec3(_Color) * toon, _Color.a);
}