#pragma once

#include <filesystem>
#include <vector>

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "assimp/scene.h"

class Model
{
public:
    Model(const std::filesystem::path& objectPath);
    ~Model() = default;

    const std::vector<std::shared_ptr<Mesh>>& GetMeshes() { return m_Meshes; }
    const std::vector<std::shared_ptr<Material>>& GetMaterials() { return m_Materials; }

private:
    void ProcessNode(glm::mat4 parentTransform, aiNode* node, const aiScene* scene);
    std::shared_ptr<Mesh> ProcessMesh(glm::mat4 transform, aiMesh* mesh, const aiScene* scene);
    static glm::mat4 ConvertMatrix(const aiMatrix4x4& matrix);

private:
    std::vector<std::shared_ptr<Mesh>> m_Meshes;
    std::vector<std::shared_ptr<Material>> m_Materials;
    std::filesystem::path m_Path;
};
