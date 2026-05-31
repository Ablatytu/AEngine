#pragma once
#include <unordered_map>
#include <string>

class UBOBuffer
{
public:
	struct UBOBufferInfo
	{
		unsigned int ID;
		uint32_t  size;
		uint32_t binding;
		std::vector<uint8_t> cpubuffer;
		bool dirty = false;
	};
	static void AddBuffer(std::string namebuffer, uint32_t  size, bool isstatic, int numberUniformBuffer);
	static void AddBuffer(std::string namebuffer, uint32_t  size, const void* ptr, bool isstatic, int numberUniformBuffer);
	static const UBOBufferInfo* existBuffer(std::string namebuffer);
	static void ChangeInfoBuffer(std::string namebuffer, uint32_t  size, const void* ptr, uint32_t offset);
	static void UpdateAll();
	static const unsigned int GetLastUBO();
private:
	static std::unordered_map<std::string, UBOBufferInfo> buffers;
	static unsigned int lastUBO;
};