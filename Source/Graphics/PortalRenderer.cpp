#include "PortalRenderer.h"

#include "ModelRenderer.h"
#include "GLFW/glfw3.h"
#include "glm/ext/matrix_transform.hpp"

uint32_t PortalRenderer::s_RecursionDepth = 2;
std::vector<PortalRenderData> PortalRenderer::s_Scene;
std::shared_ptr<Graphics::Shader> PortalRenderer::s_SceneShader;
std::shared_ptr<Graphics::VertexArray> PortalRenderer::s_PortalVAO;
std::shared_ptr<Graphics::Shader> PortalRenderer::s_PortalStencilShader;
std::shared_ptr<Graphics::Shader> PortalRenderer::s_PortalShader;
std::shared_ptr<Graphics::Texture> PortalRenderer::s_PortalTexture;

void PortalRenderer::Initialize(uint32_t recursionDepth)
{
    float portalVertices[] = {
        -0.6f, -1.125f, 0.0f, 0.0f, 0.0f,
        0.6f, -1.125f, 0.0f, 1.0f, 0.0f,
        -0.6f, 1.125f, 0.0f, 0.0f, 1.0f,
        0.6f, 1.125f, 0.0f, 1.0f, 1.0f
    };
    uint32_t portalIndices[] = {
        0, 1, 2,
        1, 3, 2
    };
    std::shared_ptr<Graphics::VertexBuffer> spriteVBO = std::make_shared<Graphics::VertexBuffer>(portalVertices, sizeof(portalVertices));
    spriteVBO->SetLayout({
        { "a_Position", Graphics::VertexDataType::Float3 },
        { "a_TexCoord", Graphics::VertexDataType::Float2 }
    });
    std::shared_ptr<Graphics::IndexBuffer> spriteIBO = std::make_shared<Graphics::IndexBuffer>(portalIndices, sizeof(portalIndices) / sizeof(uint32_t));
    s_PortalVAO = std::make_shared<Graphics::VertexArray>();
    s_PortalVAO->SetVertexBuffer(spriteVBO);
    s_PortalVAO->SetIndexBuffer(spriteIBO);

    s_PortalStencilShader = std::make_shared<Graphics::Shader>(std::filesystem::path("assets/portalStencilVert.glsl"), "assets/portalStencilFrag.glsl");
    s_PortalShader = std::make_shared<Graphics::Shader>(std::filesystem::path("assets/portalVert.glsl"), "assets/portalFrag.glsl");

    s_PortalTexture = std::make_shared<Graphics::Texture>("assets/Portal.png");

    s_RecursionDepth = recursionDepth;
}

void PortalRenderer::Shutdown()
{
    s_PortalTexture.reset();
    s_PortalShader.reset();
    s_PortalStencilShader.reset();
    s_PortalVAO.reset();
    s_SceneShader.reset();
    s_Scene.clear();
}

void PortalRenderer::Render(glm::mat4 bluePortalTransform, glm::mat4 orangePortalTransform, glm::mat4 view,
                            glm::mat4 projection)
{
    ClearStencilBuffer();

    RenderBluePortal(bluePortalTransform, orangePortalTransform, view, projection, 0);
    RenderOrangePortal(bluePortalTransform, orangePortalTransform, view, projection, 0);
}

void PortalRenderer::RenderBluePortal(glm::mat4 bluePortalTransform, glm::mat4 orangePortalTransform, glm::mat4 view,
    glm::mat4 projection, uint32_t depth)
{
    if (depth >= s_RecursionDepth) return;

    SetStencilWriteState(depth);

    RenderPortal(bluePortalTransform, view, projection, glm::vec3(0.0f, 0.0f, 1.0f));
    ClearPortalDepthBuffer(bluePortalTransform, view, projection, glm::vec3(0.0f, 0.0f, 1.0f), depth);

    glm::mat4 bluePortalView = GetPortalViewMatrix(bluePortalTransform, orangePortalTransform, view);
    glm::mat4 bluePortalProjection = GetPortalProjectionMatrix(orangePortalTransform, bluePortalView, projection);

    s_SceneShader->Bind();
    s_SceneShader->SetFloat3Uniform("u_CameraPosition", glm::inverse(bluePortalView)[3]);
    s_SceneShader->Unbind();
    ModelRenderer renderer;
    renderer.NewFrame();
    for (const auto& data : s_Scene)
    {
        renderer.Render(data.model, s_SceneShader, data.transform, bluePortalView, bluePortalProjection);
    }
    RenderBluePortal(bluePortalTransform, orangePortalTransform, bluePortalView, bluePortalProjection, depth + 1);
    glEnable(GL_STENCIL_TEST);
    renderer.EndFrame();
    ResetPortalDepthBuffer(bluePortalTransform, view, projection, glm::vec3(0.0f, 0.0f, 1.0f), depth, false);
}

void PortalRenderer::RenderOrangePortal(glm::mat4 bluePortalTransform, glm::mat4 orangePortalTransform, glm::mat4 view,
    glm::mat4 projection, uint32_t depth)
{
    if (depth >= s_RecursionDepth) return;

    SetStencilWriteState(depth);

    RenderPortal(orangePortalTransform, view, projection, glm::vec3(1.0f, 0.0f, 0.0f));
    ClearPortalDepthBuffer(orangePortalTransform, view, projection, glm::vec3(1.0f, 0.0f, 0.0f), depth);

    glm::mat4 orangePortalView = GetPortalViewMatrix(orangePortalTransform, bluePortalTransform, view);
    glm::mat4 orangePortalProjection = GetPortalProjectionMatrix(bluePortalTransform, orangePortalView, projection);

    s_SceneShader->Bind();
    s_SceneShader->SetFloat3Uniform("u_CameraPosition", glm::inverse(orangePortalView)[3]);
    s_SceneShader->Unbind();
    ModelRenderer renderer;
    renderer.NewFrame();
    for (const auto& data : s_Scene)
    {
        renderer.Render(data.model, s_SceneShader, data.transform, orangePortalView, orangePortalProjection);
    }
    RenderOrangePortal(bluePortalTransform, orangePortalTransform, orangePortalView, orangePortalProjection, depth + 1);
    glEnable(GL_STENCIL_TEST);
    renderer.EndFrame();
    ResetPortalDepthBuffer(orangePortalTransform, view, projection, glm::vec3(1.0f, 0.0f, 0.0f), depth, true);
}

void PortalRenderer::RenderPortal(glm::mat4 portalTransform, glm::mat4 view, glm::mat4 projection,
                                  glm::vec3 portalColor)
{
    s_PortalStencilShader->Bind();
    s_PortalStencilShader->SetMat4Uniform("u_Transform", portalTransform);
    s_PortalStencilShader->SetMat4Uniform("u_View", view);
    s_PortalStencilShader->SetMat4Uniform("u_Projection", projection);
    s_PortalStencilShader->SetFloat3Uniform("u_Color", portalColor);
    s_PortalTexture->BindToSlot();
    s_PortalVAO->Bind();
    glDrawElements(GL_TRIANGLES, s_PortalVAO->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
    s_PortalVAO->Unbind();
    s_PortalStencilShader->Unbind();
}

void PortalRenderer::RenderPortalFrame(glm::mat4 portalTransform, glm::mat4 view, glm::mat4 projection, bool isOrange)
{
    glEnable(GL_BLEND);
    s_PortalShader->Bind();
    s_PortalShader->SetMat4Uniform("u_Transform", portalTransform);
    s_PortalShader->SetMat4Uniform("u_View", view);
    s_PortalShader->SetMat4Uniform("u_Projection", projection);
    s_PortalShader->SetInt("u_IsOrange", isOrange);
    s_PortalShader->SetFloat("u_Time", glfwGetTime());
    s_PortalTexture->BindToSlot();
    s_PortalVAO->Bind();
    glDrawElements(GL_TRIANGLES, s_PortalVAO->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
    s_PortalVAO->Unbind();
    s_PortalShader->Unbind();
    glDisable(GL_BLEND);
}

void PortalRenderer::ClearStencilBuffer()
{
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0xFF);
    glClear(GL_STENCIL_BUFFER_BIT);
}

void PortalRenderer::SetStencilWriteState(uint32_t depth)
{
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0xFF);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    glStencilFunc(GL_EQUAL, depth, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    glStencilMask(0xFF);

    glEnable(GL_DEPTH_TEST);
}

void PortalRenderer::ClearPortalDepthBuffer(glm::mat4 portalTransform, glm::mat4 view, glm::mat4 projection,
    glm::vec3 portalColor, uint32_t depth)
{
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_ALWAYS);
    glDepthRange(1.0f, 1.0f);

    glStencilFunc(GL_EQUAL, depth + 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0x00);

    RenderPortal(portalTransform, view, projection, portalColor);

    glDepthRange(0.0f, 1.0f);
    glDepthFunc(GL_LESS);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
}

void PortalRenderer::ResetPortalDepthBuffer(glm::mat4 portalTransform, glm::mat4 view, glm::mat4 projection,
    glm::vec3 portalColor, uint32_t depth, bool isOrange)
{
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_ALWAYS);
    glDepthRange(1.0f, 1.0f);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glStencilFunc(GL_EQUAL, depth + 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
    glStencilMask(0xFF);

    RenderPortal(portalTransform, view, projection, portalColor);

    glStencilFunc(GL_EQUAL, depth, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0x00);

    glDepthRange(0.0f, 1.0f);
    glDepthFunc(GL_LESS);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    RenderPortalFrame(portalTransform, view, projection, isOrange);

    glDisable(GL_STENCIL_TEST);
}

glm::mat4 PortalRenderer::GetPortalViewMatrix(glm::mat4 entryPortalTransform, glm::mat4 exitPortalTransform, glm::mat4 view)
{
    glm::mat4 flipY = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    return view * entryPortalTransform * glm::inverse(flipY) * glm::inverse(exitPortalTransform);
}

glm::mat4 PortalRenderer::GetPortalProjectionMatrix(glm::mat4 exitPortalTransform,
    glm::mat4 newView, glm::mat4 projection)
{
    glm::vec3 worldNormal = -glm::vec3(exitPortalTransform * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
    glm::vec3 worldPoint = glm::vec3(exitPortalTransform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    glm::mat4 invView = glm::inverse(newView);
    glm::mat3 normalMatrix = glm::transpose(glm::mat3(invView));
    glm::vec3 clipNormal = glm::normalize(normalMatrix * worldNormal);
    glm::vec3 clipPoint = glm::vec3(newView * glm::vec4(worldPoint, 1.0f));

    float clipDistance = -glm::dot(clipNormal, clipPoint);
    glm::vec4 clipPlane = glm::vec4(clipNormal, clipDistance);

    glm::vec4 q;
    q.x = (glm::sign(clipPlane.x) + projection[2][0]) / projection[0][0];
    q.y = (glm::sign(clipPlane.y) + projection[2][1]) / projection[1][1];
    q.z = -1.0f;
    q.w = (1.0f + projection[2][2]) / projection[3][2];

    glm::vec4 c = clipPlane * (2.0f / glm::dot(clipPlane, q));

    glm::mat4 newProjection = projection;
    newProjection[0][2] = c.x;
    newProjection[1][2] = c.y;
    newProjection[2][2] = c.z + 1.0f;
    newProjection[3][2] = c.w;

    return newProjection;
}
