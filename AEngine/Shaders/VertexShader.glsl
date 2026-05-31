GLSLPROGRAM
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in mat4 instanceMatrix;
//4
//5
//6
layout (location = 7) in vec4 aTangent;

#include "Shaders/Core.glsl"
#include "Shaders/Shadows.glsl"

out VS_OUT 
{
 vec2 texCoords;
 vec3 Normal;
 vec3 FragPos;
 float time;
 vec4 FragPosLightSpace;
 mat3 TBN;
 vec3 TangentViewDir;
} vs_out;

uniform bool GPUInstancing;
uniform mat4 transform;

void main()
{
    mat4 model;
    if(GPUInstancing == true)
        model = instanceMatrix;
    else
        model = transform;
    gl_Position = proj * view * model * vec4(aPos, 1.0);
    vs_out.texCoords = aTexCoord;
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vs_out.Normal = normalMatrix * aNormal;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.FragPosLightSpace = GetShadowCoord(vec4(vs_out.FragPos, 1.0));
    vs_out.time = _Time;
    vec3 T = normalize(normalMatrix * aTangent.xyz);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T) * aTangent.w;
    vs_out.TBN = mat3(T, B, N);
    mat3 invTBN = transpose(vs_out.TBN);
    vs_out.TangentViewDir = invTBN * GetViewDir(vs_out.FragPos);
}  