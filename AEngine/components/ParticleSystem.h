#pragma once
#include "AEngineComponent.h"
#include <vector>

class Texture;
class Material;
class Transform;
class Camera;

struct Particle
{
	glm::vec3 Position;
	glm::vec3 Velocity;

	glm::vec4 color;

	float rotation = 0.0f;
	float rotationVelocity = 0.0f;

	float size = 1.0f;

	float life = 0.0f;
	float maxLife = 0.0f;
};

class ParticleSystem : public AEngineComponent
{
	AE_REGISTER_CLASS(ParticleSystem);
public:

	enum class Alpha
	{
		ONE,
		MINUS_SRC_ALPHA
	};

	Alpha alphaBlending = Alpha::ONE;
	AENGINE_API void Awake() override;
	AENGINE_API void Update() override;
	void Create();
	void SyncCount();
	int FindDeadParticle();
	void Respawn(Particle& particle);
	glm::vec3 RandomVec3(glm::vec3& min, glm::vec3& max);
	float RandomFloat(float& min, float& max);
	AENGINE_API void OnInspector() override;

	int maxParticles = 1000;
	float RateOverTime = 10.0f;
	
	bool looping = true;
	bool playOnStart = true;

	float lifeTimeMin = 5.0f;
	float lifeTimeMax = 5.0f;

	glm::vec3 spawnOffsetMin = glm::vec3(0.0f);
	glm::vec3 spawnOffsetMax = glm::vec3(0.0f);

	glm::vec3 velocityMin = glm::vec3(0.0f);
	glm::vec3 velocityMax = glm::vec3(0.1f, -0.5f, 0.0f);

	glm::vec3 acceleration = glm::vec3(0.0f);
	
	float startSize = 0.3f;
	float endSize = 0.1f;

	float rotationMin = 180.0f;
	float rotationMax = 180.0f;

	float rotationVelocityMin = 0.0f;
	float rotationVelocityMax = 0.0f;

	glm::vec4 startColor = glm::vec4(1.0f, 1.0f, 1.0, 0.6f);
	glm::vec4 endColor = glm::vec4(1.0f, 1.0f, 1.0, 0.6f);

	Texture* texture = nullptr;
	Material* material = nullptr;



	const std::vector<Particle>& GetParticles(Transform* camera = nullptr); 
private:
	std::vector<Particle> particles;
	Transform* transform = nullptr;

	bool playing = false;
	int cachedParticleMax = -1;
	int lastDeadParticles = 0;
	float spawnAccumulator = 0.0f;
};
