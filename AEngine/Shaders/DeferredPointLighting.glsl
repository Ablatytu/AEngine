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

flat in int LightIndex;

void main()
{

    vec2 uv = gl_FragCoord.xy / screenSize;
	PointLight pl = PointLights[LightIndex];
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
    surfinfo.uambient = vec3(0.2);
    surfinfo.albendo = albedo.rgb;
    surfinfo.udiffuse = vec3(1.0);
    surfinfo.uspecular = vec3(albedo.a);
    surfinfo.ushininess = material.r * 256.0;
    surfinfo.objectColor = vec4(1.0);
    vec3 _lighting = CalcPointLight(pl, geominfo, surfinfo);
    result = vec4(_lighting, 1.0);
    FragColor = result;
}