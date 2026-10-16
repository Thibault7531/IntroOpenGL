#include "ModelRenderer.h"

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
            tempMesh.ViewProjection = projection * view;
            tempMesh.distance = glm::distance(cameraPosition, meshPosition);
            m_TransparentMeshes.push_back(tempMesh);
        }
    }
    shader->Unbind();
}

void ModelRenderer::EndFrame()
{
    std::sort(m_TransparentMeshes.begin(), m_TransparentMeshes.end(), [](const TransparentMesh& a, const TransparentMesh& b)
    {
        return a.distance > b.distance;
    });
    for (auto mesh : m_TransparentMeshes)
    {
        mesh.Shader->Bind();
        mesh.Shader->SetMat4Uniform("u_Transform", mesh.Transform * mesh.Mesh->GetTransform());
        mesh.Shader->SetMat4Uniform("u_ViewProjection", mesh.ViewProjection);
        mesh.Shader->SetInt("u_Material.diffuseMap", 0);
        mesh.Shader->SetInt("u_Material.emissiveMap", 1);
        auto material = mesh.Model->GetMaterials()[mesh.Mesh->GetMaterialIndex()];
        material->SetupRenderState();
        mesh.Shader->SetFloat3Uniform("u_Material.ambient", material->GetAmbientColor());
        mesh.Shader->SetInt("u_Material.emissiveChannels", material->GetEmissiveTexture()->GetChannelCount());
        material->GetColorTextures()[0]->BindToSlot(0);
        material->GetEmissiveTexture()->BindToSlot(1);
        mesh.Shader->SetFloat3Uniform("u_Material.specular", material->GetSpecularColor());
        mesh.Shader->SetFloat("u_Material.shininess", material->GetShininess());
        mesh.Shader->SetFloat4Uniform("u_BaseColor", material->GetBaseColor());
        mesh.Mesh->GetVertexArray()->Bind();
        glDrawElements(GL_TRIANGLES, mesh.Mesh->GetVertexArray()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
        mesh.Mesh->GetVertexArray()->Unbind();
        mesh.Shader->Unbind();
    }
}

void ModelRenderer::ShutDown()
{
    m_TransparentMeshes.clear();
}
