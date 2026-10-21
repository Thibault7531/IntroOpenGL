#include "ModelRenderer.h"

ModelRenderer::ModelRenderer()
{
    float quad[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
    };
    uint32_t indices[] = {0, 1, 2, 2, 1, 3};
    std::shared_ptr<Graphics::VertexBuffer> quadVB = std::make_shared<Graphics::VertexBuffer>(quad, sizeof(quad));
    quadVB->SetLayout({
        {"a_Position", Graphics::VertexDataType::Float3}
    });
    m_QuadVA = std::make_shared<Graphics::VertexArray>();
    m_QuadVA->SetVertexBuffer(quadVB);
    m_QuadVA->SetIndexBuffer(std::make_shared<Graphics::IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t)));
}

void ModelRenderer::NewFrame()
{
    m_TransparentMeshes.clear();
}

void ModelRenderer::Render(std::shared_ptr<Model> model, std::shared_ptr<Graphics::Shader> shader, glm::mat4 transform, glm::mat4 view, glm::mat4 projection)
{
    shader->Bind();
    shader->SetMat4Uniform("u_ViewProjection", projection * view);
    shader->SetInt("u_Material.diffuseMap", 0);
    shader->SetInt("u_Material.emissiveMap", 1);
    glm::mat4 cameraTransform = glm::inverse(view);
    glm::vec3 cameraPosition = glm::vec3(cameraTransform[3]);
    for (auto mesh : model->GetMeshes())
    {
        if (!model->GetMaterials()[mesh->GetMaterialIndex()]->HasTransparency())
        {
            auto material = model->GetMaterials()[mesh->GetMaterialIndex()];
            material->SetupRenderState();
            shader->SetMat4Uniform("u_Transform", transform * mesh->GetTransform());
            shader->SetFloat3Uniform("u_Material.ambient", material->GetAmbientColor());
            shader->SetInt("u_Material.emissiveChannels", material->GetEmissiveTexture()->GetChannelCount());
            material->GetColorTextures()[0]->BindToSlot(0);
            material->GetEmissiveTexture()->BindToSlot(1);
            shader->SetFloat3Uniform("u_Material.specular", material->GetSpecularColor());
            shader->SetFloat("u_Material.shininess", material->GetShininess());
            shader->SetFloat4Uniform("u_BaseColor", material->GetBaseColor());
            mesh->GetVertexArray()->Bind();
            glDrawElements(GL_TRIANGLES, mesh->GetVertexArray()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
            mesh->GetVertexArray()->Unbind();
        }
        else
        {
            glm::vec3 meshPosition = glm::vec3(transform[3]);

            TransparentMesh tempMesh;
            tempMesh.Model = model;
            tempMesh.Mesh = mesh;
            tempMesh.Shader = shader;
            tempMesh.Transform = transform;
            tempMesh.View = view;
            tempMesh.Projection = projection;
            tempMesh.distance = glm::distance(cameraPosition, meshPosition);
            m_TransparentMeshes.push_back(tempMesh);
        }
    }
    shader->Unbind();
}

void ModelRenderer::EndFrame(std::shared_ptr<Graphics::Framebuffer> oitFBO, std::shared_ptr<Graphics::Shader> oitAccum, std::shared_ptr<Graphics::Shader> oitComposite)
{
    oitFBO->Bind();
    glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    float clearOne = 1.0f;
    glClearBufferfv(GL_COLOR, 0, &clearColor[0]);
    glClearBufferfv(GL_COLOR, 1, &clearOne);
    glBlitNamedFramebuffer(0, oitFBO->GetFramebufferID(), 0, 0, oitFBO->GetWidth(), oitFBO->GetHeight(), 0, 0, oitFBO->GetWidth(), oitFBO->GetHeight(), GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    for (auto mesh : m_TransparentMeshes)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        oitAccum->Bind();
        oitAccum->SetMat4Uniform("u_Transform", mesh.Transform * mesh.Mesh->GetTransform());
        oitAccum->SetMat4Uniform("u_ViewProjection", mesh.Projection * mesh.View);
        oitAccum->SetInt("u_Material.diffuseMap", 0);
        oitAccum->SetInt("u_Material.emissiveMap", 1);
        auto material = mesh.Model->GetMaterials()[mesh.Mesh->GetMaterialIndex()];
        oitAccum->SetFloat3Uniform("u_Material.ambient", material->GetAmbientColor());
        oitAccum->SetInt("u_Material.emissiveChannels", material->GetEmissiveTexture()->GetChannelCount());
        material->GetColorTextures()[0]->BindToSlot(0);
        material->GetEmissiveTexture()->BindToSlot(1);
        oitAccum->SetFloat3Uniform("u_Material.specular", material->GetSpecularColor());
        oitAccum->SetFloat("u_Material.shininess", material->GetShininess());
        oitAccum->SetFloat4Uniform("u_BaseColor", material->GetBaseColor());
        mesh.Mesh->GetVertexArray()->Bind();
        glDrawElements(GL_TRIANGLES, mesh.Mesh->GetVertexArray()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
        mesh.Mesh->GetVertexArray()->Unbind();
        oitAccum->Unbind();
    }
    oitFBO->Unbind();

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    oitComposite->Bind();
    oitComposite->SetInt("u_AccumTexture", 0);
    oitComposite->SetInt("u_RevealTexture", 1);
    oitFBO->GetColorAttachment(0)->BindToSlot(0);
    oitFBO->GetColorAttachment(1)->BindToSlot(1);
    m_QuadVA->Bind();
    glDrawElements(GL_TRIANGLES, m_QuadVA->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
    m_QuadVA->Unbind();
    oitComposite->Unbind();

}

void ModelRenderer::ShutDown()
{
    m_TransparentMeshes.clear();
}
