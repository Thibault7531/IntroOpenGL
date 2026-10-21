#pragma once
#include <vector>

#include "Framebuffer.h"
#include "Model.h"

struct PortalRenderData
{
    std::shared_ptr<Model> model;
    glm::mat4 transform;
};

class PortalRenderer
{
public:
    static void Initialize(uint32_t recursionDepth = 2);
    static void Shutdown();
    static void SetScene(std::vector<PortalRenderData> scene, std::shared_ptr<Graphics::Shader> sceneShader) { s_Scene = scene; s_SceneShader = sceneShader; }

    static void Render(glm::mat4 bluePortalTransform, glm::mat4 orangePortalTransform, glm::mat4 view, glm::mat4 projection, std::shared_ptr<Graphics::Framebuffer> oitFBO, std::shared_ptr<Graphics::Shader> oitAccumShader, std::shared_ptr<Graphics::Shader> oitCompositeShader);

private:
    static void RenderBluePortal(glm::mat4 bluePortalTransform, glm::mat4 orangePortalTransform, glm::mat4 view, glm::mat4 projection, uint32_t depth, std::shared_ptr<Graphics::Framebuffer> oitFBO, std::shared_ptr<Graphics::Shader> oitAccumShader, std::shared_ptr<Graphics::Shader> oitCompositeShader);
    static void RenderOrangePortal(glm::mat4 bluePortalTransform, glm::mat4 orangePortalTransform, glm::mat4 view, glm::mat4 projection, uint32_t depth, std::shared_ptr<Graphics::Framebuffer> oitFBO, std::shared_ptr<Graphics::Shader> oitAccumShader, std::shared_ptr<Graphics::Shader> oitCompositeShader);
    static void RenderPortal(glm::mat4 portalTransform, glm::mat4 view, glm::mat4 projection, glm::vec3 portalColor);
    static void RenderPortalFrame(glm::mat4 portalTransform, glm::mat4 view, glm::mat4 projection, bool isOrange);

    static void ClearStencilBuffer();
    static void SetStencilWriteState(uint32_t depth);
    static void ClearPortalDepthBuffer(glm::mat4 portalTransform, glm::mat4 view, glm::mat4 projection, glm::vec3 portalColor, uint32_t depth);
    static void ResetPortalDepthBuffer(glm::mat4 portalTransform, glm::mat4 view, glm::mat4 projection, glm::vec3 portalColor, uint32_t depth, bool isOrange);

    static glm::mat4 GetPortalViewMatrix(glm::mat4 entryPortalTransform, glm::mat4 exitPortalTransform, glm::mat4 view);
    static glm::mat4 GetPortalProjectionMatrix(glm::mat4 exitPortalTransform, glm::mat4 newView, glm::mat4 projection);

private:
    static uint32_t s_RecursionDepth;
    static std::vector<PortalRenderData> s_Scene;
    static std::shared_ptr<Graphics::Shader> s_SceneShader;
    static std::shared_ptr<Graphics::VertexArray> s_PortalVAO;
    static std::shared_ptr<Graphics::Shader> s_PortalStencilShader;
    static std::shared_ptr<Graphics::Shader> s_PortalShader;
    static std::shared_ptr<Graphics::Texture> s_PortalTexture;
};
