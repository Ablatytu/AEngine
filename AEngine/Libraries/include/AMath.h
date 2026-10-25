#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
inline float triangle_intersection(const glm::vec3& orig, const glm::vec3& dir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
    glm::vec3 e1 = v1 - v0;
    glm::vec3 e2 = v2 - v0;
    glm::vec3 pvec = cross(dir, e2);
    float det = glm::dot(e1, pvec);

    if (det < 1e-8 && det > -1e-8) {
        return -1.0f;
    }

    float inv_det = 1 / det;
    glm::vec3 tvec = orig - v0;
    float u = glm::dot(tvec, pvec) * inv_det;
    if (u < 0 || u > 1) {
        return -1.0f;
    }

    glm::vec3 qvec = cross(tvec, e1);
    float v = glm::dot(dir, qvec) * inv_det;
    if (v < 0 || u + v > 1) {
        return -1.0;
    }
    return dot(e2, qvec) * inv_det;
}
inline float Vector3_Distance(const glm::vec3& point1, glm::vec3 point2)
{
    float x = point2.x - point1.x;
    float y = point2.y - point1.y;
    float z = point2.z - point1.z;
    return sqrt(x * x + y * y + z * z);
}
inline float Vector3_DistanceWithoutSQRT(const glm::vec3& point1, glm::vec3 point2)
{
    float x = point2.x - point1.x;
    float y = point2.y - point1.y;
    float z = point2.z - point1.z;
    return x * x + y * y + z * z;
}
inline float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}