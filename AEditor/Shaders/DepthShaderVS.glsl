GLSLPROGRAM
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in mat4 instanceMatrix;

uniform mat4 lightSpaceMatrix;
uniform mat4 transform;
uniform bool GPUInstancing;

void main()
{
    mat4 model = GPUInstancing ? instanceMatrix : transform;
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}