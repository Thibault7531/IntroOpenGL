#pragma once

#include "Model.h"

struct TransparentMesh
{
    std::shared_ptr<Model> Model;
    std::shared_ptr<Mesh> Mesh;
    std::shared_ptr<Graphics::Shader> Shader;
    glm::mat4 Transform;
    glm::mat4 ViewProjection;
    float distance;
};

class ModelRenderer
{
public:
    ModelRenderer() = default;

    void NewFrame();
    void Render(std::shared_ptr<Model> model, std::shared_ptr<Graphics::Shader> shader, glm::mat4 transform, glm::mat4 view, glm::mat4 projection);
    void EndFrame();

    void ShutDown();

private:
    std::vector<TransparentMesh> m_TransparentMeshes;
};