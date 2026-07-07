#pragma once
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <memory>
class Transform;
class Camera;
class GameObject;
class EditorGUI;

class EditorCamera
{
public:
	EditorCamera(GameObject* _cameraObject, EditorGUI* _editorGUI);
	Transform* transform;
	Camera* camera;
	glm::vec3 direction;
	float sens = 0.2f;
	float pitch, yaw = 0.0f;
	void Update();
	glm::vec3 CalcMouseRay();
	GameObject* FindClosestObject();
private:
	GameObject* cameraObject = nullptr;
	EditorGUI* editorGUI;
};