#include "TestComponent.h"
#include "GameObject.h"
#include "Vector3.h"
#include <iostream>

void TestComponent::Start()
{
	std::cout << "TESTCOMPONENTSTART\n";
	transform = GetGameObject().GetAComponent<Transform>();

}
void TestComponent::Update(float deltaTime)
{
	if (move)
	{
		if (transform)
		{
			Vector3 rotation = transform->GetRotation();
			transform->Rotate(Vector3{ 30.0f * deltaTime, 30.0f * deltaTime, 30.0f * deltaTime });
			//transform->SetPosition(vec3Test);
			transform->SetPosition(Vector3( transform->GetPosition().x - (speed * deltaTime), transform->GetPosition().y - (speed * deltaTime), transform->GetPosition().z - (speed * deltaTime)));
		}
	}
}
void TestComponent::OnInspector()
{
	AE_REGISTER_FIELD(move);
	AE_REGISTER_FIELD(speed);
	AE_REGISTER_FIELD(CPLUSPLUS);
	AE_REGISTER_FIELD(vec3Test);
	AE_REGISTER_FIELD(testInt);
	AE_SET_RANGE(testInt, 0, 100);
	AE_SET_RANGE(speed, 0.0f, 10.0f);
}

AE_REGISTER_COMPONENT(TestComponent);