#include "FramebufferManager.h"
#include "../scene/AssetManager.h"
FramebufferManager& FramebufferManager::Get()
{
	static FramebufferManager manager;
	return manager;
}
Framebuffer* FramebufferManager::Add(std::string name, const std::string& Vertexshader, const std::string& FragmentShader, const std::vector<Framebuffer::TypeFrameBuffer>& typeFrameBuffer, bool resizable, int width, int height, const std::vector<Framebuffer::TypeFilter>& typeFilter)
{
	Shader* shader = nullptr;
	if(Vertexshader != "__NULL__" || FragmentShader != "__NULL__")
		shader = AssetManager::LoadShader(Vertexshader, FragmentShader);
	std::unique_ptr<Framebuffer> framebuffer = std::make_unique<Framebuffer>(shader, typeFrameBuffer, resizable, width, height, typeFilter);
	Framebuffer* framebufferptr = framebuffer.get();
	framebuffers[name] = std::move(framebuffer);
	return framebufferptr;
}
void FramebufferManager::Resize(int width, int height)
{
	for (auto& it : framebuffers)
		it.second->Resize(width, height);
}
void FramebufferManager::Delete(std::string name)
{
	auto it = framebuffers.find(name);
	if (it != framebuffers.end())
		framebuffers.erase(it);
}
Framebuffer* FramebufferManager::GetFramebuffer(std::string name)
{
	auto it = framebuffers.find(name);
	if (it != framebuffers.end())
		return it->second.get();
	return nullptr;
}
std::vector<Framebuffer*> FramebufferManager::GetFramebuffers()
{
	std::vector<Framebuffer*> buffers;
	for (auto& buffer : framebuffers)
	{
		buffers.push_back(buffer.second.get());
	}
	return buffers;
}