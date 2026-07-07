GLSLPROGRAM
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT 
{
 vec2 texCoords;
 vec3 Normal;
 vec3 FragPos;
 float time;
 vec4 FragPosLightSpace;
 mat3 TBN;
 vec3 TangentViewDir;
} gs_in[];

out VS_OUT 
{
    vec2 texCoords;
    vec3 Normal;
    vec3 FragPos;
    float time;
    vec4 FragPosLightSpace;
    mat3 TBN;
    vec3 TangentViewDir;
} gs_out;

vec3 GetNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}
vec4 explode(vec4 pos, vec3 norm, int number_gs_in)
{
	float magnitude = 3.0;
	vec3 direction = norm * ((sin(gs_in[0].time) + 1.0) / 2.0) * magnitude;
	gs_out.FragPos = gs_in[number_gs_in].FragPos + direction;
	return pos + vec4(direction, 0.0);
}

void main()
{
	vec3 normal = GetNormal();
	gl_Position = explode(gl_in[0].gl_Position, normal, 0);
	gs_out.texCoords = gs_in[0].texCoords;
    gs_out.Normal = gs_in[0].Normal;
    gs_out.time = gs_in[0].time;
    gs_out.FragPosLightSpace = gs_in[0].FragPosLightSpace;
    gs_out.TBN = gs_in[0].TBN;
    gs_out.TangentViewDir = gs_in[0].TangentViewDir;
	EmitVertex();

	gl_Position = explode(gl_in[1].gl_Position, normal, 1);
	gs_out.texCoords = gs_in[1].texCoords;
    gs_out.Normal = gs_in[1].Normal;
    gs_out.time = gs_in[1].time;
    gs_out.FragPosLightSpace = gs_in[1].FragPosLightSpace;
    gs_out.TBN = gs_in[1].TBN;
    gs_out.TangentViewDir = gs_in[1].TangentViewDir;
	EmitVertex();

    gl_Position = explode(gl_in[2].gl_Position, normal, 2);
	gs_out.texCoords = gs_in[2].texCoords;
    gs_out.Normal = gs_in[2].Normal;
    gs_out.time = gs_in[2].time;
    gs_out.FragPosLightSpace = gs_in[2].FragPosLightSpace;
    gs_out.TBN = gs_in[2].TBN;
    gs_out.TangentViewDir = gs_in[2].TangentViewDir;
	EmitVertex();
	EndPrimitive();
}
