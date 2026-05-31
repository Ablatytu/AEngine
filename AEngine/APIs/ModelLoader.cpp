#include "ModelLoader.h"
#include "../core/ErrorManager.h"
#include "../Renderer/Mesh.h"

void Model::loadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        CriticalGameError("Failed to load 3D model");
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}
void Model::processNode(aiNode* node, const aiScene* scene)
{

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;

            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;

            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    if (mesh->mMaterialIndex >= 0)
    {
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
    }
    for (unsigned int i = 0; i < indices.size(); i += 3)
    {
        Vertex& pos1 = vertices[indices[i]];
        Vertex& pos2 = vertices[indices[i+1]];
        Vertex& pos3 = vertices[indices[i+2]];

        glm::vec2& uv1 = vertices[indices[i]].TexCoords;
        glm::vec2& uv2 = vertices[indices[i+1]].TexCoords;
        glm::vec2& uv3 = vertices[indices[i+2]].TexCoords;

        glm::vec3 edge1 = pos2.Position - pos1.Position;
        glm::vec3 edge2 = pos3.Position - pos1.Position;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 Tangent = glm::vec3(0.0f);
        Tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        Tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        Tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        glm::vec3 Bitangent = glm::vec3(0.0f);
        Bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        Bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        Bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        float w = (glm::dot(glm::cross(pos1.Normal, Tangent), Bitangent) < 0.0f) ? -1.0f : 1.0f;

        pos1.Tangent += glm::vec4(Tangent, 0.0f);
        pos1.Bitangent += Bitangent;
        pos2.Tangent += glm::vec4(Tangent, 0.0f);
        pos2.Bitangent += Bitangent;
        pos3.Tangent += glm::vec4(Tangent, 0.0f);
        pos3.Bitangent += Bitangent;

        pos1.Tangent.w = w;
        pos2.Tangent.w = w;
        pos3.Tangent.w = w;
    }
    return Mesh(vertices, indices);
}
