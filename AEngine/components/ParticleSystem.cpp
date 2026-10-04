#include "ParticleSystem.h"
#include "../Renderer/GameObject.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Material.h"
#include "../core/Window.h"
#include "Transform.h"
#include <random>
#include <AMath.h>

void ParticleSystem::Awake()
{
	transform = gameObject->GetComponent<Transform>();
	Create();
}

void ParticleSystem::Update()
{
	SyncCount();

	const float deltaTime = Time::deltaTime;

	for (Particle& particle : particles)
	{
		if (particle.life <= 0.0f)
			continue;

		particle.life -= deltaTime;

		if (particle.life <= 0.0f)
			continue;

		particle.Velocity += acceleration * deltaTime;
		particle.Position += particle.Velocity * deltaTime;

		particle.rotation += particle.rotationVelocity * deltaTime;

		float t = 1.0f - particle.life / particle.maxLife;

		particle.size = glm::mix(startSize, endSize, t);

		particle.color = glm::mix(startColor, endColor, t);
	}

	if (!playing || RateOverTime <= 0.0f)
		return;

	spawnAccumulator += RateOverTime * deltaTime;

	int spawnCount = static_cast<int>(spawnAccumulator);

	spawnAccumulator -= static_cast<float>(spawnCount);

	for (int i = 0; i < spawnCount; ++i)
	{
		int index = FindDeadParticle();

		if (index == -1)
			break;
		
		Respawn(particles[index]);
	}
}

void ParticleSystem::Create()
{
	SyncCount();

	spawnAccumulator = 0.0f;
	lastDeadParticles = 0;

	playing = playOnStart;
}

int ParticleSystem::FindDeadParticle()
{
	if (particles.empty())
		return -1;

	for (size_t i = lastDeadParticles; i < particles.size(); ++i)
	{
		if (particles[i].life <= 0.0f)
		{
			lastDeadParticles = i + 1;

			if (lastDeadParticles >= particles.size())
				lastDeadParticles = 0;

			return static_cast<int>(i);
		}
	}

	for (size_t i = 0; i < lastDeadParticles; ++i)
	{
		if (particles[i].life <= 0.0f)
		{
			lastDeadParticles = i + 1;

			if (lastDeadParticles >= particles.size())
				lastDeadParticles = 0;

			return static_cast<int>(i);
		}
	}

	return -1;
}

void ParticleSystem::SyncCount()
{
	maxParticles = std::clamp(maxParticles, 0, 50000);

	if (maxParticles == cachedParticleMax)
		return;

	particles.resize(maxParticles);

	cachedParticleMax = maxParticles;
	lastDeadParticles = 0;
}

void ParticleSystem::Respawn(Particle& particle)
{
	particle.life = RandomFloat(lifeTimeMin, lifeTimeMax);
	particle.maxLife = particle.life;

	particle.Position = transform->position + RandomVec3(spawnOffsetMin, spawnOffsetMax);

	particle.Velocity = RandomVec3(velocityMin, velocityMax);

	particle.rotation = RandomFloat(rotationMin, rotationMax);

	particle.rotationVelocity = RandomFloat(rotationVelocityMin, rotationVelocityMax);

	particle.size = startSize;
	particle.color = startColor;
}

glm::vec3 ParticleSystem::RandomVec3(glm::vec3& min, glm::vec3& max)
{
	return glm::vec3(RandomFloat(min.x, max.x), RandomFloat(min.y, max.y), RandomFloat(min.z, max.z));
}
float ParticleSystem::RandomFloat(float& min, float& max)
{
	if (min > max)
		std::swap(min, max);
	static std::mt19937 generator{ std::random_device{}() };
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(generator);
}

const std::vector<Particle>& ParticleSystem::GetParticles(Transform* camera)
{
	return particles;
}
void ParticleSystem::OnInspector()
{
	RegisterField(maxParticles);
	Range(maxParticles, 0.0f, 50000.0f);

	RegisterField(RateOverTime);
	Range(RateOverTime, 0, 1000);


	RegisterField(looping);
	RegisterField(playOnStart);

	RegisterField(lifeTimeMin);
	Range(lifeTimeMin, 0.0f, 1000.0f);

	RegisterField(lifeTimeMax);
	Range(lifeTimeMax, 0.0f, 1000.0f);

	RegisterField(spawnOffsetMin);
	RegisterField(spawnOffsetMax);
	RegisterField(velocityMin);
	RegisterField(velocityMax);
	RegisterField(acceleration);

	RegisterField(startSize);
	Range(startSize, 0.0f, 1000.0f);

	RegisterField(endSize);
	Range(endSize, 0.0f, 1000.0f);

	RegisterField(rotationMin);
	Range(rotationMin, 0.0f, 360.0f);

	RegisterField(rotationMax);
	Range(rotationMax, 0.0f, 360.0f);

	RegisterField(rotationVelocityMin);
	Range(rotationVelocityMin, 0.0f, 1000.0f);

	RegisterField(rotationVelocityMax);
	Range(rotationVelocityMax, 0.0f, 1000.0f);

	RegisterField(startColor);
	RegisterField(endColor);

	RegisterField(material);
}















AE_REGISTER_COMPONENT(ParticleSystem);