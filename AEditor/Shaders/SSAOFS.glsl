GLSLPROGRAM
#version 330 core

#include "Shaders/Core.glsl"

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;
uniform vec3 samples[64];
uniform vec2 screenSize;

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;
in vec2 TexCoord;
out float FragColor;
void main()
{
	vec2 noiseScale = vec2(screenSize.x / 4.0, screenSize.y / 4.0);
	vec3 fragPos   = (view * vec4(texture(gPosition, TexCoord).rgb, 1.0)).xyz;
	vec3 normal    = normalize(mat3(view) * texture(gNormal, TexCoord).rgb);
	vec3 randomVec = texture(texNoise, TexCoord * noiseScale).xyz;  
	vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN       = mat3(tangent, bitangent, normal);  
	float occlusion = 0.0;
	for(int i = 0; i < kernelSize; ++i)
	{
		vec3 samplePos = TBN * samples[i];
		samplePos = fragPos + samplePos * radius; 
		vec4 offset = vec4(samplePos, 1.0);
		offset      = proj * offset;   
		offset.xyz /= offset.w;             
		offset.xyz  = offset.xyz * 0.5 + 0.5; 
		float sampleDepth = (view * vec4(texture(gPosition, offset.xy).rgb, 1.0)).z; 
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;  
	}  
	occlusion = 1.0 - (occlusion / kernelSize);
	FragColor = pow(occlusion, 1.0);  
}