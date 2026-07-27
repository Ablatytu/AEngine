#pragma once
#include "Framebuffer.h"
#include "../core/AEngineAPI.h"
#include <unordered_map>
#include <string>
#include <memory>

class FramebufferManager
{
public:
	AENGINE_API static FramebufferManager& Get();
	AENGINE_API Framebuffer* Add(std::string name, Shader* frameshader, const std::vector<Framebuffer::TypeFrameBuffer>& typeFrameBuffer = { Framebuffer::TypeFrameBuffer::LDR }, bool resizable = true, int width = -1, int height = -1, const std::vector<Framebuffer::TypeFilter>& typeFilter = {Framebuffer::TypeFilter::Liner});
	AENGINE_API void Resize(int width, int height);
	AENGINE_API void Delete(std::string name);
	AENGINE_API Framebuffer* GetFramebuffer(std::string name);
	AENGINE_API std::vector<Framebuffer*> GetFramebuffers();
private:
	std::unordered_map<std::string, std::unique_ptr<Framebuffer>> framebuffers;
};