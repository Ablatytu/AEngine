#include "Buffer.h"
#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <iostream>
std::unordered_map<std::string, UBOBuffer::UBOBufferInfo> UBOBuffer::buffers;
unsigned int UBOBuffer::lastUBO = 0;
void UBOBuffer::AddBuffer(std::string namebuffer, uint32_t  size, bool isstatic, int numberUniformBuffer)
{
	std::cout << "StartUBOBuffer + " << namebuffer;
	UBOBuffer::UBOBufferInfo newbuffer{};
	newbuffer.size = size;
	newbuffer.cpubuffer.resize(size);
	newbuffer.dirty = true;
	glGenBuffers(1, &newbuffer.ID);
	glBindBuffer(GL_UNIFORM_BUFFER, newbuffer.ID);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, isstatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	newbuffer.binding = numberUniformBuffer;
	glBindBufferRange(GL_UNIFORM_BUFFER, newbuffer.binding, newbuffer.ID, 0, size);
	buffers[namebuffer] = newbuffer;
	std::cout << " EndUBOBuffer" << std::endl;
	lastUBO++;
}
void UBOBuffer::AddBuffer(std::string namebuffer, uint32_t  size, const void* ptr, bool isstatic, int numberUniformBuffer)
{
	UBOBuffer::UBOBufferInfo newbuffer{};
	newbuffer.size = size;
	newbuffer.cpubuffer.resize(size);
	glGenBuffers(1, &newbuffer.ID);
	glBindBuffer(GL_UNIFORM_BUFFER, newbuffer.ID);
	glBufferData(GL_UNIFORM_BUFFER, size, ptr, isstatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	newbuffer.binding = numberUniformBuffer;
	glBindBufferRange(GL_UNIFORM_BUFFER, newbuffer.binding, newbuffer.ID, 0, size);
	buffers[namebuffer] = newbuffer;
	ChangeInfoBuffer(namebuffer, size, ptr, 0);
	lastUBO++;
}
const UBOBuffer::UBOBufferInfo* UBOBuffer::existBuffer(std::string namebuffer)
{
	if (buffers.find(namebuffer) != buffers.end())
		return &buffers.find(namebuffer)->second;
	else
		return nullptr;
}
void UBOBuffer::ChangeInfoBuffer(std::string namebuffer, uint32_t  size, const void* ptr, uint32_t offset)
{
	if (existBuffer(namebuffer))
	{
		auto buf = buffers.find(namebuffer);
		if (offset + size > buf->second.cpubuffer.size())
			return;
		memcpy(buf->second.cpubuffer.data() + offset, ptr, size);
		buf->second.dirty = true;
	}
}
void UBOBuffer::UpdateAll()
{
	for (auto& t : buffers)
	{
		if (t.second.dirty)
		{
			t.second.dirty = false;
			glBindBuffer(GL_UNIFORM_BUFFER, t.second.ID);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, t.second.size, t.second.cpubuffer.data());
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}
}
const unsigned int UBOBuffer::GetLastUBO()
{
	return lastUBO;
}