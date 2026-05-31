GLSLPROGRAM
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D SceneTexture;
uniform sampler2D PreviousExposure;

uniform float mipLevel;
uniform float targetBrightness;
uniform float minExposure;
uniform float maxExposure;
uniform float adaptationSpeed;
uniform float deltaTime;

void main()
{
	vec3 avgColor = textureLod(SceneTexture, vec2(0.5), mipLevel).rgb;

	float lum = dot(avgColor, vec3(0.2126, 0.7152, 0.0722));

	float targetExposure = clamp(targetBrightness / max(lum, 0.01), minExposure, maxExposure);

	float previousExposure = texture(PreviousExposure, vec2(0.5)).r;

	float t = 1.0 - exp(-adaptationSpeed * deltaTime);

	float finalExposure = mix(previousExposure, targetExposure, t);

	FragColor = vec4(finalExposure, 0.0, 0.0, 1.0);
}