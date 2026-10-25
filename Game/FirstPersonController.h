#include "AComponent.h"
#include "AComponentRef.h"
#include "Transform.h"

class FirstPersonController : public AComponent
{
	AE_REGISTER_CLASS(FirstPersonController);
public:
	void Update(float deltaTime) override;
	void Start() override;
	void OnInspector();

private:
	AComponentRef<Transform> transform;
	float speed = 3.0f;
	float sens = 1.0f;
	float pitch = 1.0f, yaw = 1.0f;
};