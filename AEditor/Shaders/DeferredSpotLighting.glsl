GLSLPROGRAM
#version 330 core
#include "Shaders/Shadows.glsl"
#include "Shaders/Lighting.glsl"
#include "Shaders/Core.glsl"

out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMaterial;
uniform vec2 screenSize;
uniform sampler2D SSAO;

flat in int LightIndex;

void main()
{
    vec2 uv = gl_FragCoord.xy / screenSize;
	SpotLight sl = SpotLights[LightIndex];
	vec3 fragPos = texture(gPosition, uv).rgb;
    vec3 normal = texture(gNormal, uv).rgb;
    vec4 albedo = texture(gAlbedo, uv);
    vec4 material = texture(gMaterial, uv);
    vec4 result = vec4(1.0, 1.0, 1.0, 1.0);
    GeometryInfo geominfo;
    geominfo.Normal = normal;
    geominfo.FragPos = fragPos;
    geominfo.camPos = camPos;
    SurfaceInfo surfinfo;
    surfinfo.albedo = albedo.rgb;
    surfinfo.metallic = material.r;
    surfinfo.roughness = material.g;
    surfinfo.ao = material.b * texture(SSAO, uv).r;
    vec3 _lighting = CalcSpotLight(sl, geominfo, surfinfo);
    result = vec4(_lighting, 1.0);
    FragColor = result;
}