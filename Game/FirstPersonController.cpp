#include "FirstPersonController.h"
#include "GameObject.h"
#include "Input.h"

void FirstPersonController::Update(float deltaTime)
{
    float mouseX = Input::GetAxis("Mouse X") * sens;
    float mouseY = Input::GetAxis("Mouse Y") * sens;

    pitch -= mouseY;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    yaw += mouseX;

    transform->SetRotation(Vector3(pitch, yaw, 0.0f));

    float xMove = Input::GetAxis("Horizontal");
    float yMove = Input::GetAxis("Vertical");

    Vector3 forwardPlayer = transform->GetForward();
    Vector3 rightPlayer = transform->GetRight();

    forwardPlayer.y = 0.0f;
    rightPlayer.y = 0.0f;

    forwardPlayer.Normalize();
    rightPlayer.Normalize();

    transform->SetPosition((transform->GetPosition() + (rightPlayer * xMove + forwardPlayer * yMove) * deltaTime * speed));
}

void FirstPersonController::Start()
{
	transform = GetGameObject().GetAComponent<Transform>();
}
void FirstPersonController::OnInspector()
{
	AE_REGISTER_FIELD(speed);
	AE_SET_RANGE(speed, 0.0f, 100.0f);
	AE_REGISTER_FIELD(sens);
	AE_SET_RANGE(sens, 0.0f, 10.0f);
}

AE_REGISTER_COMPONENT(FirstPersonController)