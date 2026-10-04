#include "RenderSystem2D.h"
#include "Shader.h"
#include "../components/Camera.h"
#include "../components/SpriteRenderer2D.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Material.h"
#include <map>
#include <glad/glad.h>
#include <memory>

namespace RenderSystem2D
{
    glm::mat4 viewProjMatrix = glm::mat4(1.0f);
	void rs2D_Init()
	{
        float vertices[] = {
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f
        };

        glGenBuffers(1, &quad.VBO);
        glGenVertexArrays(1, &quad.VAO);

        glBindBuffer(GL_ARRAY_BUFFER, quad.VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(quad.VAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(0);
	}

    void rs2D_CalcView(RenderContext2D& context)
    {
        if (context.perspective)
            viewProjMatrix = context.camera->projection * context.camera->viewMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));
        else
        {
            viewProjMatrix = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1000.0f, 1000.0f);
        }
    }

    void rs2D_RenderSprites(RenderContext2D& context, std::vector<Batch2D>& batches2D)
    {
        for (Batch2D& batch : batches2D)
        {
            batch.material->GetShader()->Update();
            batch.material->UpdateMaterial();
            if (batch.texture)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, batch.texture->GetTexturePixels());
                batch.material->GetShader()->setInt("SpriteImage", 0);
            }
            batch.material->GetShader()->setMat4("proj", viewProjMatrix);
            for (size_t i = 0; i < batch.models.size(); i++)
            {
                batch.material->GetShader()->setVec4("SpriteColor", batch.colors[i]);
                batch.material->GetShader()->setMat4("model", batch.models[i]);
                glBindVertexArray(quad.VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);
            }
        }
    }

    void rs2D_Collect2DBatches(RenderContext2D& context, std::vector<Batch2D>& batches2D)
    {
        std::map<Batch2D, size_t> newbatches2D;
        for (SpriteRenderer2D* sprite : context.sprites)
        {
            if (sprite->material && sprite->material->GetShader())
            {
                Batch2D newBatch{};
                newBatch.material = sprite->material;
                newBatch.texture = sprite->texture;
                auto findedBatch2D = newbatches2D.find(newBatch);
                Batch2D* pushedBatch = nullptr;
                if (findedBatch2D == newbatches2D.end())
                {
                    batches2D.push_back(newBatch);
                    pushedBatch = &batches2D.back();
                    newbatches2D[newBatch] = batches2D.size() - 1;
                }
                else
                {
                    pushedBatch = &batches2D[findedBatch2D->second];
                }
                pushedBatch->models.push_back(sprite->transform->GetModelMatrix());
                pushedBatch->colors.push_back(glm::vec4(sprite->color.r, sprite->color.g, sprite->color.b, 1.0f));
            }
        }
    }
    void rs2D_Render(RenderContext2D& context)
    {
        rs2D_CalcView(context);
        std::vector<Batch2D> batches2D;
        rs2D_Collect2DBatches(context, batches2D);
        rs2D_RenderSprites(context, batches2D);
    }

    void rs2D_Start(Framebuffer* sceneHDR)
    {
        sceneHDR->Bind();
    }
    void rs2D_End()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}