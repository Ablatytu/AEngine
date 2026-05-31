Properties
{
    uambient = (Vector3, (0.2, 0.2, 0.2))
    udiffuse = (Vector3, (1.0, 1.0, 1.0))
    uspecular = (Vector3, (0.5, 0.5, 0.5))
    [Range(2.0, 256.0)] ushininess = (Float, 64.0)
    objectColor = (Color, (1.0, 1.0, 1.0, 1.0))
    [Range(0.0, 1.0)] coefficientCubemap = (Float, 0.0)
    texture_diffuse1 = (Texture2D, white)
    coefficientNormal = (Float, 0.0)
    InvertNormalY = (Bool, False)
    normalMap = (Texture2D, normal)
    Cubemap = (TextureCube, white)
    heightMap = (Texture2D, black) 
    [Range(0.0, 0.1)] height_scale = (Float, 0.0)
}

RenderPath "Deferred"

GLSLPROGRAM
#version 330 core
#include "Shaders/AEngineGBufferFS.glsl"
#include "Shaders/Core.glsl"

uniform vec3 uambient;
uniform vec3 udiffuse;
uniform vec3 uspecular;
uniform float ushininess;
uniform vec4 objectColor;
uniform float coefficientCubemap;
uniform sampler2D texture_diffuse1;
uniform samplerCube Cubemap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;
uniform float coefficientNormal;
uniform bool InvertNormalY;
uniform float height_scale;
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

vec3 CalcCubemap(vec3 normal)
{
    vec3 I = normalize(fs_in.FragPos - camPos);
    vec3 R = reflect(I, normalize(normal));
    vec3 color = texture(Cubemap, R).rgb;
    return color;
}
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDirTBN)
{
    vec3 NviewDirTBN = normalize(viewDirTBN);
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), NviewDirTBN)));  
    int numLayersI = int(numLayers);
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    vec2 p = NviewDirTBN.xy * height_scale;
    vec2 deltaTexCoords = p / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(heightMap, currentTexCoords).r;

    for(int i = 0; i < 32; i++)
    {
        if(i >= numLayersI || currentDepthMapValue < currentLayerDepth)
            break;
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(heightMap, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(heightMap, prevTexCoords).r - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}
void main()
{
            
    vec4 albedo = vec4(1.0, 1.0, 1.0, 1.0);
    vec2 texCoords = fs_in.texCoords;
    if(height_scale > 0.0)
    {
        texCoords = ParallaxMapping(texCoords, fs_in.TangentViewDir);
        if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;
    }
    vec4 _texture = texture(texture_diffuse1, texCoords);
    vec3 normal = fs_in.Normal;
    if(coefficientNormal > 0.0)
    {
        normal = texture(normalMap, texCoords).rgb;
        normal = normal * 2.0 - 1.0;
        if(InvertNormalY)
            normal.y = -normal.y;
        normal.xy *= coefficientNormal;
        normal = normalize(fs_in.TBN * normal);
    }
    float alpha = _texture.a * objectColor.a;
    vec3 objectColorFinal = _texture.rgb * vec3(objectColor);
    if(coefficientCubemap > 0.0)
    {
        vec3 objectCubemap = CalcCubemap(normal);
        vec3 objectColorWithLightingTexture_Cubemap = mix(_texture.rgb, objectCubemap, coefficientCubemap);
        albedo = vec4(objectColorWithLightingTexture_Cubemap, alpha);
    }
    else
    {
        albedo = vec4(objectColorFinal, alpha);
    }
    gPosition = fs_in.FragPos;
    gNormal = normal;
    gAlbedo = vec4(albedo.rgb, uspecular.r);
    gMaterial = vec4(ushininess / 256.0, 0.0, 0.0, 1.0);
}
        