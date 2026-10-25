#include "AComponent.h"
#include "Camera.h"
#include "AComponentRef.h"
#include "Material.h"

class PostEffectCameraBreakout : public AComponent
{
	AE_REGISTER_CLASS(PostEffectCameraBreakout);
public:
	void Start() override;
	bool OnRenderImage(RenderImage* source, RenderImage* destination, Camera* cam) override;

	bool Confuse = false;
	bool Chaos = false;
	bool Shake = false;
private:
	AComponentRef<Camera> cam;
	Material matCam;
};