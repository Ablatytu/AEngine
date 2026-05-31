#include "Mesh.h"
#include <glad/glad.h>
#include "../scene/AssetManager.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
    this->vertices = vertices;
    this->indices = indices;
    
    CreateMesh();
}
void Mesh::CreateMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glGenBuffers(1, &instanceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    glm::mat4 EmptyMatrixInstance = glm::mat4(1.0f);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), &EmptyMatrixInstance, GL_DYNAMIC_DRAW);
    for (int instanceMatrix = 3; instanceMatrix < 7; instanceMatrix++)
    {
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(instanceMatrix);
        glVertexAttribPointer(instanceMatrix, 4, GL_FLOAT, GL_FALSE, 64, (void*)((instanceMatrix - 3) * 16));
        glVertexAttribDivisor(instanceMatrix, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    glBindVertexArray(0);
}
void Mesh::Draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void Mesh::DrawInstance(unsigned int count, const void* modelMatrix)
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
    if (count != instancebuffersize)
    {
        instancebuffersize = count;
        glBufferData(GL_ARRAY_BUFFER, instancebuffersize * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(glm::mat4), modelMatrix);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0, count);
    glBindVertexArray(0);
}