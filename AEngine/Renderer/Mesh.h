#ifndef MESH_H
#define MESH_H
#include <vector>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <string> 
#include "Material.h"
#include "../core/AEngineAPI.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec4 Tangent = glm::vec4(0.0);
    glm::vec3 Bitangent = glm::vec3(0.0);
};

class AENGINE_API Mesh
{
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    Mesh() = default;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;
    unsigned int instanceBuffer;
    int IDMesh = 0;
    std::string basepathModel = "";
    size_t instancebuffersize = 1;
	void CreateMesh();
    void Draw();
    void DrawInstance(unsigned int count, const void* modelMatrix);
};

#endif