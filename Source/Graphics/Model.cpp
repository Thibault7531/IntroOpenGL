#include "Model.h"

#include <csignal>

#include "assimp/Importer.hpp"
#include <assimp/postprocess.h>

#include "Texture.h"
#include "glm/glm.hpp"

Model::Model(const std::filesystem::path& path)
    : m_Path(path)
{
    Assimp::Importer importer;
    const auto scene = importer.ReadFile(path.string(), aiProcess_Triangulate);

    if (!scene) std::printf("Error loading model: %s\n", importer.GetErrorString());

    ProcessNode(glm::mat4(1.0f), scene->mRootNode, scene);

    for (uint32_t i = 0; i < scene->mNumMaterials; i++)
    {
        m_Materials.push_back(std::make_shared<Material>(path.parent_path(), scene->mMaterials[i], scene));
    }
}

void Model::ProcessNode(glm::mat4 parentTransform, aiNode* node, const aiScene* scene)
{
    glm::mat4 localTransform = ConvertMatrix(node->mTransformation);
    glm::mat4 transform = parentTransform * localTransform;
    for (uint32_t i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(ProcessMesh(transform, mesh, scene));
    }
    for (uint32_t i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(transform, node->mChildren[i], scene);
    }
}

std::shared_ptr<Mesh> Model::ProcessMesh(glm::mat4 transform, aiMesh* mesh, const aiScene* scene)
{
    return std::make_shared<Mesh>(transform, mesh, scene);
}

glm::mat4 Model::ConvertMatrix(const aiMatrix4x4& matrix)
{
    glm::mat4 to;
    to[0][0] = matrix.a1; to[1][0] = matrix.a2; to[2][0] = matrix.a3; to[3][0] = matrix.a4;
    to[0][1] = matrix.b1; to[1][1] = matrix.b2; to[2][1] = matrix.b3; to[3][1] = matrix.b4;
    to[0][2] = matrix.c1; to[1][2] = matrix.c2; to[2][2] = matrix.c3; to[3][2] = matrix.c4;
    to[0][3] = matrix.d1; to[1][3] = matrix.d2; to[2][3] = matrix.d3; to[3][3] = matrix.d4;
    return to;
}

