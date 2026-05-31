#pragma once
#include <vector>

class Shader;

class Framebuffer
{
public:
    enum class TypeFrameBuffer
    {
        LDR,
        HDR,
        R16F,
        RGBA16F,
        RGBA
    };
    enum class TypeFilter
    {
        Liner,
        Nearest
    };
    Framebuffer(Shader* shader, const std::vector<TypeFrameBuffer>& typeFrameBuffer = {TypeFrameBuffer::LDR}, bool resizable = true, int width = -1, int height = -1, const std::vector<TypeFilter>& typeFilter = {TypeFilter::Liner});
    void UseShader();
    void Drawframebuffer();
    void Bind();
    void UnBind();
    void Resize(int width, int height);
    void ChangeShader(Shader* shader);
    unsigned int GetFBO();
    unsigned int GetColorBuffer(unsigned int MCBNumber);
    Shader* GetShader();
private:
    float quadVertices[24] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int framebuffer;
    unsigned int quadVAO;
    std::vector<unsigned int> ColorBuffer;
    std::vector<TypeFilter> typeFilter = { TypeFilter::Liner };
    std::vector<TypeFrameBuffer> typeFrameBuffer = {TypeFrameBuffer::LDR};
    unsigned int rbo;
    bool resizable = true;
    Shader* screenShader;
};