#include "LightManager.h"
#include "../components/Light.h"
#include <vector>

void LightManager::AddLight(Light* light)
{
	lights.push_back(light);
}
void LightManager::RemoveLight(Light* light)
{
	lights.erase(std::remove(lights.begin(), lights.end(), light), lights.end());
}
void LightManager::EraseLights()
{
	lights.clear();
}
const std::vector<Light*>& LightManager::GetLights() const
{
	return lights;
}