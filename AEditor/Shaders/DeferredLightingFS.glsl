GLSLPROGRAM
#version 330 core

#include "Shaders/Lighting.glsl"
#include "Shaders/Core.glsl"
#include "Shaders/Shadows.glsl"

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMaterial;
uniform sampler2D SSAO;

float LinearDepth(float depth)
{	
    float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}
float CalcFogDeferred(vec3 fragPos)
{
    if (fog.fogEnabled == 0)
        return 0.0;

    float dist = length(camPos - fragPos);
    float fogFactor = clamp(dist / far, 0.0, 1.0);
    return pow(fogFactor, 1.4);
}

void main()
{
    vec3 fragPos = texture(gPosition, TexCoord).rgb;
    vec3 normal = texture(gNormal, TexCoord).rgb;
    vec4 albedo = texture(gAlbedo, TexCoord);
    vec4 material = texture(gMaterial, TexCoord);

	vec4 result = vec4(1.0, 1.0, 1.0, 1.0);
    GeometryInfo geominfo;
    geominfo.Normal = normal;
    geominfo.FragPos = fragPos;
    geominfo.camPos = camPos;
    geominfo.shadowCoord = GetShadowCoord(vec4(fragPos, 1.0));
    SurfaceInfo surfinfo;
    surfinfo.uambient = vec3(0.03);
    surfinfo.albedo = albedo.rgb;
    surfinfo.metallic = material.r;
    surfinfo.roughness = material.g;
    surfinfo.ao = material.b * texture(SSAO, TexCoord).r;
    vec3 _lighting = vec3(0.0);
    for(int i = 0; i < countDirLight; i++)
        _lighting += CalcDirLight(DirectionalLights[i], geominfo, surfinfo);
    vec3 objectColorFinal = mix(_lighting, fog.colorFog, CalcFogDeferred(fragPos));
    result = vec4(objectColorFinal + calcAmbient(geominfo, surfinfo), 1.0);
    FragColor = result;
}