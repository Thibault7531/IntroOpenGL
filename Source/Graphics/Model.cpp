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
    const auto scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_PreTransformVertices);

    if (!scene) std::printf("Error loading model: %s\n", importer.GetErrorString());

    ProcessNode(scene->mRootNode, scene);

    for (uint32_t i = 0; i < scene->mNumMaterials; i++)
    {
        m_Materials.push_back(std::make_shared<Material>(path.parent_path(), scene->mMaterials[i], scene));
    }
}

void Model::Render(std::shared_ptr<Graphics::Shader> shader, glm::mat4 transform, glm::mat4 viewProjection)
{
    shader->Bind();
    shader->SetMat4Uniform("u_Transform", transform);
    shader->SetMat4Uniform("u_ViewProjection", viewProjection);
    shader->SetInt("u_Texture", 0);
    std::vector<std::shared_ptr<Mesh>> transparentMeshes;
    for (auto mesh : GetMeshes())
    {
        if (GetMaterials()[mesh->GetMaterialIndex()]->GetOpacity() == 1.0f)
        {
            auto material = GetMaterials()[mesh->GetMaterialIndex()];
            material->SetupRenderState();
            shader->SetFloat4Uniform("u_BaseColor", material->GetColor());
            mesh->GetVertexArray()->Bind();
            glDrawElements(GL_TRIANGLES, mesh->GetVertexArray()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
            mesh->GetVertexArray()->Unbind();
        }
        else
        {
            transparentMeshes.push_back(mesh);
        }
    }
    for (auto mesh : transparentMeshes)
    {
        auto material = GetMaterials()[mesh->GetMaterialIndex()];
        material->SetupRenderState();
        shader->SetFloat4Uniform("u_BaseColor", material->GetColor());
        mesh->GetVertexArray()->Bind();
        glDrawElements(GL_TRIANGLES, mesh->GetVertexArray()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
        mesh->GetVertexArray()->Unbind();
    }
    shader->Unbind();
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (uint32_t i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(ProcessMesh(mesh, scene));
    }
    for (uint32_t i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

std::shared_ptr<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    return std::make_shared<Mesh>(mesh, scene);
}
