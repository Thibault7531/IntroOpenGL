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

    void Render(std::shared_ptr<Graphics::Shader> shader, glm::mat4 transform, glm::mat4 viewProjection);

    const std::vector<std::shared_ptr<Mesh>>& GetMeshes() { return m_Meshes; }
    const std::vector<std::shared_ptr<Material>>& GetMaterials() { return m_Materials; }

private:
    void ProcessNode(aiNode* node, const aiScene* scene);
    std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

private:
    std::vector<std::shared_ptr<Mesh>> m_Meshes;
    std::vector<std::shared_ptr<Material>> m_Materials;
    std::filesystem::path m_Path;
};
