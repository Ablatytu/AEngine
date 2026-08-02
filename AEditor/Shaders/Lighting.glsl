#define MAX_SPOT 128
#define MAX_DIR 10
#define MAX_POINT 192

uniform sampler2D shadowMap;
uniform samplerCube shadowMapCube;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

struct DirectionalLight
{
float intensive;
vec3 direction;
vec3 lightColor;
};

struct PointLight
{
float intensive; 
vec3 position; 
vec3 lightColor; 
float far_plane; 
float castShadow;
float Radius;
};
struct SpotLight
{
float intensive;
vec3 position;
vec3  direction;
float cutOff;
vec3 lightColor;
float outerCutOff;
};
struct Fog
{
int fogEnabled;
vec3 colorFog;
};

layout (std140) uniform u_DirectionalLight
{
    int countDirLight;
    DirectionalLight DirectionalLights[MAX_DIR];
};
layout (std140) uniform u_PointLight
{
    int countPointLight;
    PointLight PointLights[MAX_POINT];
};
layout (std140) uniform u_SpotLight
{
    int countSpotLight;
    SpotLight SpotLights[MAX_SPOT];
};
layout (std140) uniform u_Fog
{
    Fog fog;
};
struct SurfaceInfo
{
vec3 albedo;
float metallic;
float roughness;
float ao;
};
struct GeometryInfo
{
vec3 Normal;
vec3 FragPos;
vec3 camPos;
vec4 shadowCoord;
};
vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 
const float PI = 3.14159265359;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
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
vec3 CalcPBRBRDF(vec3 N, vec3 V, vec3 L, vec3 radiance, vec3 albedo, float metallic, float roughness)
{
	vec3 H = normalize(V + L);

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

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
	return (kD * albedo / PI + specular) * radiance * NdotL;
}

float ShadowCalc(vec4 fragPosLightSpace, GeometryInfo geominfo, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0)
        return 0.0;
    float currentDepth = projCoords.z;
    float shadow = 0.0;
    vec2 texelsize = 1.0/textureSize(shadowMap, 0);
    float bias = max(0.005 * (1.0 - dot(normalize(geominfo.Normal), lightDir)), 0.0005);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelsize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}
float ShadowCalcPoint(GeometryInfo geominfo, vec3 lightPos, float far_plane)
{
    vec3 fragToLight = geominfo.FragPos - lightPos; 
    float currentDepth = length(fragToLight); 
    float shadow = 0.0; 
    float bias = 0.05; 
    float samples = 20;
    float viewDistance = length(geominfo.camPos - geominfo.FragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowMapCube, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane; 
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return shadow;
}
vec3 CalcPointLight(PointLight pointLight, GeometryInfo geominfo, SurfaceInfo surfinfo)
{
    vec3 N = normalize(geominfo.Normal);
    vec3 V = normalize(geominfo.camPos - geominfo.FragPos);

    vec3 L = normalize(pointLight.position - geominfo.FragPos);
    float distance = length(pointLight.position - geominfo.FragPos);
    if(distance > pointLight.Radius)
        return vec3(0.0);
    float attenuation = 1.0 / max(distance * distance, 0.01);
    
    float fade = 1.0 - pow(distance / pointLight.Radius, 4.0);
    fade = max(fade, 0.0);
    fade *= fade;

    vec3 radiance = pointLight.lightColor * pointLight.intensive * attenuation * fade;
    vec3 result = CalcPBRBRDF(N, V, L, radiance, surfinfo.albedo, surfinfo.metallic, surfinfo.roughness);
    float shadow = 0.0;
    if(pointLight.castShadow > 0.5)
        shadow = ShadowCalcPoint(geominfo, pointLight.position, pointLight.far_plane);
    return result * (1.0 - shadow);
}
vec3 CalcSpotLight(SpotLight spotLight, GeometryInfo geominfo, SurfaceInfo surfinfo)
{
    vec3 N = normalize(geominfo.Normal);
    vec3 V = normalize(geominfo.camPos - geominfo.FragPos);

    vec3 L = normalize(spotLight.position - geominfo.FragPos);
    float distance = length(spotLight.position - geominfo.FragPos);

    float attenuation = 1.0 / max(distance * distance, 0.01);

    float theta = dot(L, normalize(-spotLight.direction));
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    float spotIntensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
    vec3 radiance = spotLight.lightColor * spotLight.intensive * attenuation * spotIntensity;
    
    return CalcPBRBRDF(N, V, L, radiance, surfinfo.albedo, surfinfo.metallic, surfinfo.roughness);
}
vec3 CalcDirLight(DirectionalLight directionalLight, GeometryInfo geominfo, SurfaceInfo surfinfo)
{
    vec3 N = normalize(geominfo.Normal);
    vec3 V = normalize(geominfo.camPos - geominfo.FragPos);

    vec3 L = normalize(-directionalLight.direction);

    vec3 radiance = directionalLight.lightColor * directionalLight.intensive;

    vec3 result = CalcPBRBRDF(N, V, L, radiance, surfinfo.albedo, surfinfo.metallic, surfinfo.roughness);
    float shadow = ShadowCalc(geominfo.shadowCoord, geominfo, L);

    return result * (1.0 - shadow);
}

vec3 calcAmbient(GeometryInfo geominfo, SurfaceInfo surfinfo)
{
    vec3 N = normalize(geominfo.Normal);
    vec3 V = normalize(geominfo.camPos - geominfo.FragPos);
    vec3 R = reflect(-V, N); 
    vec3 F0 = mix(vec3(0.04), surfinfo.albedo, surfinfo.metallic);
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, surfinfo.roughness);;
    vec3 kS =  F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - surfinfo.metallic;	  
    vec3 irradiance = texture(irradianceMap, geominfo.Normal).rgb;
    vec3 diffuse = irradiance * surfinfo.albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, surfinfo.roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), surfinfo.roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * surfinfo.ao;
    return ambient;
}

vec3 FragmentStandartPBR(GeometryInfo geominfo, SurfaceInfo surfinfo)
{
    vec3 result = vec3(0.0);
    for(int i = 0; i < min(countSpotLight, 4); i++)
        result += CalcSpotLight(SpotLights[i], geominfo, surfinfo);
    for(int i = 0; i < min(countDirLight, 4); i++)
        result += CalcDirLight(DirectionalLights[i], geominfo, surfinfo);
    for(int i = 0; i < min(countPointLight, 4); i++)
        result += CalcPointLight(PointLights[i], geominfo, surfinfo);

    return result + calcAmbient(geominfo, surfinfo);
}
