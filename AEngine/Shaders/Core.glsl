layout (std140) uniform u_Matrices
{
    mat4 view;
    mat4 proj;
};
layout (std140) uniform u_CameraInfo
{
    float near;
    float far;
    vec3 camPos;
};
layout (std140) uniform u_Time
{
    float _Time;
};
vec3 GetViewDir(vec3 FragPos)
{
    return (camPos - FragPos);
}