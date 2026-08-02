#pragma once
#include <vector>

class Light;

class LightManager
{
public:
	void AddLight(Light* light);
	void RemoveLight(Light* light);
	void EraseLights();
	const std::vector<Light*>& GetLights() const;
private:
	std::vector<Light*> lights;
};