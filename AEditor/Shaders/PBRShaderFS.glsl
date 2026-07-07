Properties
{
	AlbedoTexture = (Texture2D, white)
	[Range(0.0, 1.0)] metallic = (Float, 0.0)
	[Range(0.0, 1.0)] roughness = (Float, 0.0)
	[Range(0.0, 1.0)] ao = (Float, 0.0)
}
GLSLPROGRAM
#version 330 core

#include "Shaders/Core.glsl"
#include "Shaders/Lighting.glsl"

out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D AlbedoTexture;

uniform float metallic;
uniform float roughness;
uniform float ao;

const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float ndotH = max(dot(N, H), 0.0);
	float NdotH2 = ndotH * ndotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float ndotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(ndotL, roughness);

	return ggx1 * ggx2;
}

void main()
{

	vec3 F0 = vec3(0.04);
	vec3 albedo = texture(AlbedoTexture, TexCoords).rgb;
	F0 = mix(F0, albedo, metallic);

	vec3 N = normalize(Normal);
	vec3 V = normalize(camPos - FragPos);
	vec3 Lo = vec3(0.0);
	for(int i = 0; i < 4; ++i)
	{	
		vec3 L = normalize(PointLights[i].position - FragPos);
		vec3 H = normalize(V + L);

		float distance = length(PointLights[i].position - FragPos);
		float attenuation = 1.0 / (distance * distance);

		vec3 radiance = PointLights[i].lightColor * PointLights[i].intensive * attenuation;
		float NDF = DistributionGGX(N, H, roughness);       
		float G   = GeometrySmith(N, V, L, roughness);        
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
		vec3 specular = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;

		kD *= 1.0 - metallic;
		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}
	vec3 ambient = vec3(0.03) * albedo * ao;
	vec3 color = ambient + Lo;
	FragColor = vec4(color, 1.0);
}