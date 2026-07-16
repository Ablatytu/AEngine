#pragma once
#include <vector>
#include "../core/AEngineAPI.h"
class Shader;

class AENGINE_API Framebuffer
{
public:
    enum class TypeFrameBuffer
    {
        LDR,
        HDR,
        R16F,
        RGBA16F,
        RGBA,
        RED
    };
    enum class TypeFilter
    {
        Liner,
        Nearest
    };
    Framebuffer(Shader* shader, const std::vector<TypeFrameBuffer>& typeFrameBuffer = {TypeFrameBuffer::LDR}, bool resizable = true, int width = -1, int height = -1, const std::vector<TypeFilter>& typeFilter = {TypeFilter::Liner});
    void UseShader();
    void Bind();
    void UnBind();
    void Resize(int _width, int _height);
    int GetWidth();
    int GetHeight();
    void ChangeShader(Shader* shader);
    unsigned int GetFBO();
    unsigned int GetColorBuffer(unsigned int MCBNumber);
    Shader* GetShader();
private:
    int width = 0, height = 0;
    unsigned int framebuffer;
    std::vector<unsigned int> ColorBuffer;
    std::vector<TypeFilter> typeFilter = { TypeFilter::Liner };
    std::vector<TypeFrameBuffer> typeFrameBuffer = {TypeFrameBuffer::LDR};
    unsigned int rbo;
    bool resizable = true;
    Shader* screenShader;
};