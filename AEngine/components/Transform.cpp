#include "Transform.h"

void Transform::UpdateMatrices()
{
	glm::mat4 pos = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 rot = glm::toMat4(rotation);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), this->scale);
	matrixModel = pos * rot * scale;
}
void Transform::OnInspector()
{
	RegisterField(position);
	RegisterField(rotation);
	RegisterField(scale);
}
glm::mat4 Transform::GetModelMatrix()
{
	return matrixModel;
}
glm::vec3 Transform::forward()
{
	return rotation * glm::vec3(0.0f, 0.0f, -1.0f);
}
glm::vec3 Transform::right()
{
	return rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}
glm::vec3 Transform::up()
{
	return rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}
glm::quat Quaternion::Euler(float x, float y, float z)
{
	return glm::quat(glm::radians(glm::vec3(x, y, z)));
}