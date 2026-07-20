Properties
{
    uambient = (Vector3, (0.2, 0.2, 0.2))
    [Range(0.0, 1.0)] metallic = (Float, 0.0)
	[Range(0.0, 1.0)] roughness = (Float, 0.5)
	[Range(0.0, 1.0)] ao = (Float, 1.0)
    objectColor = (Color, (1.0, 1.0, 1.0, 1.0))
    texture_diffuse1 = (Texture2D, white)
    coefficientNormal = (Float, 0.0)
    InvertNormalY = (Bool, False)
    normalMap = (Texture2D, normal)
    Cubemap = (TextureCube, white)
    heightMap = (Texture2D, black) 
    [Range(0.0, 0.1)] height_scale = (Float, 0.0)
}

RenderPath "Forward"

GLSLPROGRAM
#version 330 core
#include "Shaders/Lighting.glsl"
#include "Shaders/Core.glsl"

out vec4 FragColor;
uniform vec3 uambient;
uniform vec3 udiffuse;
uniform vec3 uspecular;
uniform float ushininess;
uniform vec4 objectColor;
uniform sampler2D texture_diffuse1;
uniform sampler2D normalMap;
uniform sampler2D heightMap;
uniform float coefficientNormal;
uniform bool InvertNormalY;
uniform float height_scale;
uniform float metallic;
uniform float roughness;
uniform float ao;

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
float LinearDepth(float depth)
{	
    float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}
float CalcFog()
{
    if (fog.fogEnabled == 0)
        return 0.0;
    float depth = LinearDepth(gl_FragCoord.z) / far;
    depth = clamp(depth, 0.0, 1.0);
    return pow(depth, 1.4);
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
    vec4 result = vec4(1.0, 1.0, 1.0, 1.0);
    vec2 texCoords = fs_in.texCoords;
    if(height_scale > 0.0)
    {
        texCoords = ParallaxMapping(texCoords, fs_in.TangentViewDir);
        if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
            discard;
    }
    vec4 _texture = texture(texture_diffuse1, texCoords);
    GeometryInfo geominfo;
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
    geominfo.Normal = normal;
    geominfo.FragPos = fs_in.FragPos;
    geominfo.camPos = camPos;
    geominfo.shadowCoord = fs_in.FragPosLightSpace;
    SurfaceInfo surfinfo;
    surfinfo.uambient = uambient;
    surfinfo.albedo = _texture.rgb * vec3(objectColor);
    surfinfo.metallic = metallic;
    surfinfo.roughness = roughness;
    surfinfo.ao = ao;
    vec3 _lighting = FragmentStandartPBR(geominfo, surfinfo);
    float alpha = _texture.a * objectColor.a;
    vec3 objectColorFinal = mix(_lighting, fog.colorFog, CalcFog());
    result = vec4(objectColorFinal, alpha);
    FragColor = result;
} 