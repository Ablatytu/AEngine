#define MAXSHADOWS 1

layout (std140) uniform LightMatrices
{
	mat4 lightSpaceMatrix[MAXSHADOWS];
};
vec4 GetShadowCoord(vec4 fragPos)
{
	return lightSpaceMatrix[0] * fragPos;
}