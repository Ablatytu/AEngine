#pragma once
#include "../Renderer/Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include "../core/AEngineAPI.h"
class AENGINE_API  Model
{
public:
    std::vector<Mesh> meshes;
    Model(const std::string& path)
    {
        loadModel(path);
    }
private:
    std::string directory;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};