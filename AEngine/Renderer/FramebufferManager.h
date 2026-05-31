#pragma once
#include "Framebuffer.h"
#include <unordered_map>
#include <string>
#include <memory>
class FramebufferManager
{
public:
	static FramebufferManager& Get();
	Framebuffer* Add(std::string name, const std::string& Vertexshader, const std::string& FragmentShader, const std::vector<Framebuffer::TypeFrameBuffer>& typeFrameBuffer = { Framebuffer::TypeFrameBuffer::LDR }, bool resizable = true, int width = -1, int height = -1, const std::vector<Framebuffer::TypeFilter>& typeFilter = {Framebuffer::TypeFilter::Liner});
	void Resize(int width, int height);
	void Delete(std::string name);
	Framebuffer* GetFramebuffer(std::string name);
	std::vector<Framebuffer*> GetFramebuffers();
private:
	std::unordered_map<std::string, std::unique_ptr<Framebuffer>> framebuffers;
};