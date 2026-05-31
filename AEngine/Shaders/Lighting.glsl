#define MAX_SPOT 128
#define MAX_DIR 10
#define MAX_POINT 192

uniform sampler2D shadowMap;
uniform samplerCube shadowMapCube;

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
float constant;
float linear;
float quadratic;
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
float constant;
float linear;
float quadratic;
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
vec3 uambient;
vec3 udiffuse;
vec3 uspecular;
float ushininess;
vec4 objectColor;
vec3 albendo;
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
    vec3 norm = normalize(geominfo.Normal);
    vec3 lightDir = normalize(pointLight.position - geominfo.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    float dist = length(pointLight.position - geominfo.FragPos);
    if(dist < pointLight.Radius)
    {
        float attenuation = 1.0 / (pointLight.constant + pointLight.linear * dist + pointLight.quadratic * (dist * dist));
        vec3 viewDir = normalize(geominfo.camPos - geominfo.FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), surfinfo.ushininess);

        vec3 ambient = surfinfo.uambient * pointLight.lightColor * pointLight.intensive * surfinfo.albendo;
        vec3 diffuse = diff * surfinfo.udiffuse * pointLight.lightColor * pointLight.intensive * surfinfo.albendo;
        vec3 specular = spec * surfinfo.uspecular * pointLight.lightColor * pointLight.intensive;

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
    
        float shadow = 0.0;
        if(pointLight.castShadow > 0.5)
            shadow = ShadowCalcPoint(geominfo, pointLight.position, pointLight.far_plane);
        vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular));
        return result;
    }
    return vec3(0.0);
}
vec3 CalcSpotLight(SpotLight spotLight, GeometryInfo geominfo, SurfaceInfo surfinfo)
{
    vec3 lightDir = normalize(spotLight.position - geominfo.FragPos);
    float dist = length(spotLight.position - geominfo.FragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * dist + spotLight.quadratic * (dist * dist));
    float theta = dot(lightDir, normalize(-spotLight.direction));
    float epsilon   = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);   
    vec3 result = vec3(0.0);
    vec3 ambient = surfinfo.uambient * spotLight.lightColor * spotLight.intensive * surfinfo.albendo;
    vec3 norm = normalize(geominfo.Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * surfinfo.udiffuse) * spotLight.lightColor * spotLight.intensive * surfinfo.albendo;
    vec3 viewDir = normalize(geominfo.camPos - geominfo.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), surfinfo.ushininess);
    vec3 specular = (surfinfo.uspecular * spec) * spotLight.lightColor * spotLight.intensive;
    diffuse *= intensity;
    specular *= intensity;
    ambient *= intensity;
    diffuse *= attenuation;
    specular *= attenuation;
    ambient *= attenuation;
    result = (ambient + diffuse + specular);  
    return result;
}
vec3 CalcDirLight(DirectionalLight directionalLight, GeometryInfo geominfo, SurfaceInfo surfinfo)
{
    vec3 lightDir = normalize(-directionalLight.direction);
    vec3 ambient = surfinfo.uambient * directionalLight.lightColor * directionalLight.intensive * surfinfo.albendo;
    vec3 norm = normalize(geominfo.Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * surfinfo.udiffuse) * directionalLight.lightColor * directionalLight.intensive * surfinfo.albendo;
    vec3 viewDir = normalize(geominfo.camPos - geominfo.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), surfinfo.ushininess);
    vec3 specular = (surfinfo.uspecular * spec) * directionalLight.lightColor * directionalLight.intensive;
    float shadow = ShadowCalc(geominfo.shadowCoord, geominfo, lightDir);
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular));
    return result;
}
vec3 FragmentBlinn(GeometryInfo geominfo, SurfaceInfo surfinfo)
{
    vec3 result = vec3(0.0);
    for(int i = 0; i < min(countSpotLight, 4); i++)
        result += CalcSpotLight(SpotLights[i], geominfo, surfinfo);
    for(int i = 0; i < min(countDirLight, 4); i++)
        result += CalcDirLight(DirectionalLights[i], geominfo, surfinfo);
    for(int i = 0; i < min(countPointLight, 4); i++)
        result += CalcPointLight(PointLights[i], geominfo, surfinfo);
    return result;
}
