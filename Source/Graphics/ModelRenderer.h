#pragma once

#include "Framebuffer.h"
#include "Model.h"

struct TransparentMesh
{
    std::shared_ptr<Model> Model;
    std::shared_ptr<Mesh> Mesh;
    std::shared_ptr<Graphics::Shader> Shader;
    glm::mat4 Transform;
    glm::mat4 View;
    glm::mat4 Projection;
    float distance;
};

class ModelRenderer
{
public:
    ModelRenderer();

    void NewFrame();
    void Render(std::shared_ptr<Model> model, std::shared_ptr<Graphics::Shader> shader, glm::mat4 transform, glm::mat4 view, glm::mat4 projection);
    void EndFrame(std::shared_ptr<Graphics::Framebuffer> oitFBO, std::shared_ptr<Graphics::Shader> oitAccum, std::shared_ptr<Graphics::Shader> oitComposite);

    void ShutDown();

private:
    std::vector<TransparentMesh> m_TransparentMeshes;

    std::shared_ptr<Graphics::VertexArray> m_QuadVA;
};