#include <chrono>
#include <iostream>
#include "imgui_impl_glfw.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "imgui_impl_opengl3.h"
#include "Camera/CameraController.h"
#include "Camera/KeyFrameAnimation.h"
#include "Camera/OrbitCamera.h"
#include "Camera/PerspectiveCamera.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/norm.hpp"
#include "Graphics/Buffers.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/Model.h"
#include "Graphics/ModelRenderer.h"
#include "Graphics/PortalRenderer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexArray.h"
#include "Windowing/Utils.h"
#include "Windowing/Window.h"
#include "Windowing/WindowManager.h"

static PerspectiveCameraController camera{16.f/9.f, glm::radians(60.f)};
static OrbitCamera orbitCamera{glm::radians(60.0f), 1920.0f/1080.0f, 0.1f, 1000.0f};
static std::shared_ptr<Graphics::Framebuffer> oitFBO;

void OnKeyPressedEvent(Window* window, int key, int scancode, int action, int mode)
{
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) window->SetShouldClose();
    if (action == GLFW_PRESS && key == GLFW_KEY_F11)
    {
        if (window->IsFullscreen()) window->SetFullscreen(false);
        else window->SetFullscreen(true);
    }
}

void OnCursorPosEvent(Window* window, double xpos, double ypos)
{
    if (ImGui::GetIO().WantCaptureMouse) return;
    camera.OnMouseMoveEvent(window, xpos, ypos);
    orbitCamera.OnMouseMoved(window, xpos, ypos);
}

void OnMouseScrollEvent(Window* window, double xoffset, double yoffset)
{
    orbitCamera.OnMouseScrolled(xoffset, yoffset);
}

void OnFramebufferResized(Window*, int width, int height)
{
    glViewport(0, 0, width, height);

    camera.OnWindowResize(width, height);
    orbitCamera.OnWindowResized(width, height);
    if (oitFBO) oitFBO->Resize(width, height);
}

int main()
{
    Utils::Init();

    Window* p_Window = WindowManager::Get().CreateMainWindow({"OpenGL", 1920, 1080});
    p_Window->SetKeyCallback(OnKeyPressedEvent);
    p_Window->SetCursorPosCallback(OnCursorPosEvent);
    p_Window->SetMouseScrollCallback(OnMouseScrollEvent);
    p_Window->SetFramebufferResizeCallback(OnFramebufferResized);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(p_Window->GetNativeWindow(), true);
    ImGui_ImplOpenGL3_Init();

    std::shared_ptr<Graphics::Shader> shader = std::make_shared<Graphics::Shader>(std::filesystem::path("assets") / "triangleVert.glsl", std::filesystem::path("assets") / "triangleFrag.glsl");

    camera.GetCamera().SetProjection(glm::radians(60.f), 16.f / 9.f, 0.001f, 100.f);
    //camera.GetCamera().setPosition({-5.5f, 4.f, 1.8f});
    camera.GetCamera().setPosition({-9.76f, 4.35f, 17.69f});
    //camera.GetCamera().SetRotation({0.f, -220.f, 0.f});
    camera.GetCamera().SetRotation({-89.9f, -406.6f, 0.f});
    orbitCamera.SetDistance(5.0f);

    uint8_t whiteTextureData[] = {255, 255, 255, 255};
    std::shared_ptr<Graphics::Texture> whiteTexture = std::make_shared<Graphics::Texture>(1, 1);
    whiteTexture->SetData(&whiteTextureData, GL_RGBA);

    std::shared_ptr<Graphics::Texture> crateTexture = std::make_shared<Graphics::Texture>("assets/wooden_crate.jpg");

    float cubeVertices[] = {
        // Front face
        -0.5f, -0.5f, 0.5f, 0.f, 0.f,
        0.5f, -0.5f, 0.5f, 1.f, 0.f,
        -0.5f,  0.5f, 0.5f, 0.f, 1.f,
        0.5f,  0.5f, 0.5f, 1.f, 1.f,

        // Right face
        0.5f, -0.5f, 0.5f, 0.f, 0.f,
        0.5f, -0.5f, -0.5f, 1.f, 0.f,
        0.5f,  0.5f, 0.5f, 0.f, 1.f,
        0.5f,  0.5f, -0.5f, 1.f, 1.f,

        // Back face
        0.5f, -0.5f, -0.5f, 0.f, 0.f,
        -0.5f, -0.5f, -0.5f, 1.f, 0.f,
        0.5f,  0.5f, -0.5f, 0.f, 1.f,
        -0.5f,  0.5f, -0.5f, 1.f, 1.f,

        // Left face
        -0.5f, -0.5f, -0.5f, 0.f, 0.f,
        -0.5f, -0.5f, 0.5f, 1.f, 0.f,
        -0.5f,  0.5f, -0.5f, 0.f, 1.f,
        -0.5f,  0.5f, 0.5f, 1.f, 1.f,

        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f,

        // Top face
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f
    };
    uint32_t cubeIndices[] = {
        0, 1, 2,
        1, 3, 2,

        4, 5, 6,
        5, 7, 6,

        8, 9, 10,
        9, 11, 10,

        12, 13, 14,
        13, 15, 14,

        16, 17, 18,
        17, 19, 18,

        20, 21, 22,
        21, 23, 22
    };
    std::shared_ptr<Graphics::VertexBuffer> cubeVbo = std::make_shared<Graphics::VertexBuffer>(cubeVertices, sizeof(cubeVertices));
    cubeVbo->SetLayout({
            {"a_Position", Graphics::VertexDataType::Float3},
            {"a_TexCoord", Graphics::VertexDataType::Float2}
    });
    std::shared_ptr<Graphics::IndexBuffer> cubeIbo = std::make_shared<Graphics::IndexBuffer>(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t));

    std::shared_ptr<Graphics::VertexArray> cubeVao = std::make_shared<Graphics::VertexArray>();
    cubeVao->SetVertexBuffer(cubeVbo);
    cubeVao->SetIndexBuffer(cubeIbo);

    std::shared_ptr<Graphics::Shader> modelShader = std::make_shared<Graphics::Shader>(std::filesystem::path("assets/ModelVert.glsl"), "assets/ModelFrag.glsl");
    std::shared_ptr<Graphics::Shader> oitAccumShader = std::make_shared<Graphics::Shader>(std::filesystem::path("assets/ModelVert.glsl"), "assets/oitAccum.glsl");
    std::shared_ptr<Graphics::Shader> oitCompositeShader = std::make_shared<Graphics::Shader>(std::filesystem::path("assets/oitCompositeVert.glsl"), "assets/oitCompositeFrag.glsl");

    std::shared_ptr<Model> portalGun = std::make_shared<Model>("assets/PortalGun2/scene.gltf");
    std::shared_ptr<Model> map = std::make_shared<Model>("assets/Portal Map 3/scene.gltf");
    std::shared_ptr<Model> cube = std::make_shared<Model>("assets/cube_companion/scene.gltf");

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    struct PointLight
    {
        glm::vec3 position;
        glm::vec3 ambient{0.2f};
        glm::vec3 diffuse{1.0f};
        glm::vec3 specular{0.5f};
        float constant = 1.0f;
        float linear = 0.22f;
        float quadratic = 0.2f;
    };
    constexpr int pointLightCount = 13;
    PointLight lights[pointLightCount];
    lights[0].position = {-10.39f, 6.2f, 6.03f};
    lights[0].linear = 0.22f;
    lights[0].quadratic = 0.2f;
    lights[1].position = {-11.61f, 3.58f, 4.86f};
    lights[1].diffuse = glm::vec3(0.0f, 0.522f, 0.667f) * 0.35f;
    lights[1].linear = 1.0f;
    lights[1].quadratic = 0.8f;
    lights[2].position = {-3.38f, 8.4f, 6.11f};
    lights[2].linear = 0.0f;
    lights[2].quadratic = 0.012f;
    lights[3].position = {-14.03f, 4.89f, 7.11f};
    lights[3].linear = 0.07f;
    lights[3].quadratic = 0.017f;
    lights[4].position = {-9.75f, 3.29f, 13.49f};
    lights[4].diffuse = glm::vec3(0.0f, 0.522f, 0.667f) * 0.35f;
    lights[4].linear = 1.0f;
    lights[4].quadratic = 0.8f;
    lights[5].position = {-9.75f, 3.29f, 13.91f};
    lights[5].diffuse = glm::vec3(0.0f, 0.522f, 0.667f) * 0.35f;
    lights[5].linear = 1.0f;
    lights[5].quadratic = 0.8f;
    lights[6].position = {-9.75f, 2.94f, 17.69f};
    lights[6].diffuse = glm::vec3(0.0f, 0.522f, 0.667f) * 0.5f;
    lights[6].linear = 0.07f;
    lights[6].quadratic = 0.005f;
    lights[7].position = {-16.85f, 3.96f, 18.2f};
    lights[7].diffuse = glm::vec3(0.0f, 0.522f, 0.667f) * 0.35f;
    lights[7].linear = 1.0f;
    lights[7].quadratic = 0.8f;
    lights[8].position = {-16.85f, 5.21f, 16.81f};
    lights[8].diffuse = glm::vec3(1.0f, 1.0f, 1.0f) * 0.35f;
    lights[8].linear = 1.0f;
    lights[8].quadratic = 0.8f;
    lights[9].position = {-16.85f, 5.21f, 16.12f};
    lights[9].diffuse = glm::vec3(1.0f, 1.0f, 1.0f) * 0.35f;
    lights[9].linear = 1.0f;
    lights[9].quadratic = 0.8f;
    lights[10].position = {-17.16f, 3.28f, 16.45f};
    lights[10].diffuse = glm::vec3(0.0f, 0.522f, 0.667f) * 0.35f;
    lights[10].linear = 1.0f;
    lights[10].quadratic = 0.8f;
    lights[11].position = {-4.85f, 8.45f, 15.85f};
    lights[11].linear = 0.0f;
    lights[11].quadratic = 0.012f;
    lights[12].position = {-15.82f, 2.94f, 20.0f};
    lights[12].diffuse = {1.0f, 0.75f, 0.8f};
    lights[12].constant = 1.0f;
    lights[12].linear = 1.0f;
    lights[12].quadratic = 0.0f;

    struct DirectionalLight
    {
        glm::vec3 direction;
        glm::vec3 ambient{0.2f};
        glm::vec3 diffuse{0.0f};
        glm::vec3 specular{0.0f};
    };
    DirectionalLight dirLight;
    dirLight.direction = glm::vec3(0.0f, -1.0f, 0.0f);

    glm::vec3 portalGunPosition = glm::vec3(0.16f, -0.22f, -0.3f);
    glm::vec3 cubePosition = glm::vec3(-15.81f, 2.93f, 20.1f);
    float cubeRotation = 0.0f;
    float cubeScale = 0.5f;

    glm::vec3 bluePortalPosition = glm::vec3(-11.245f, 4.27f, 7.86f);
    float bluePortalRotation = 0.0f;
    glm::vec3 bluePortal2Position = glm::vec3(-12.805f, 3.61f, 21.32f);
    float bluePortal2Rotation = 0.0f;

    glm::vec3 orangePortalPosition = glm::vec3(-15.83f, 3.63f, 6.09f);
    float orangePortalRotation = glm::radians(270.0f);
    glm::vec3 orangePortal2Position = glm::vec3(-12.8f, 3.63f, 14.03f);
    float orangePortal2Rotation = glm::radians(180.0f);
    //glm::vec3 orangePortalPosition = glm::vec3(-11.16f, 4.17f, 6.095f);
    //float orangePortalRotation = glm::radians(180.0f);

    PortalRenderer::Initialize(5);

    ModelRenderer worldRenderer;

    std::vector<KeyFrame> keyFrames;
    keyFrames.push_back({5.0f, glm::vec3(-5.5f, 5.8f, 1.5f), glm::vec3(-8.5f, 5.5f, 4.3f)});
    keyFrames.push_back({9.0f, glm::vec3(-6.3f, 3.7f, 3.8f), glm::vec3(-8.6f, 4.0f, 4.9f)});
    keyFrames.push_back({14.0f, glm::vec3(-12.6f, 4.9f, 1.6f), glm::vec3(-14.0f, 4.8f, 7.9f)});
    keyFrames.push_back({19.0f, glm::vec3(-15.1f, 4.3f, 4.1f), glm::vec3(-15.8f, 4.0f, 6.0f)});
    keyFrames.push_back({29.0f, glm::vec3(-14.0f, 4.2f, 5.9f), glm::vec3(-15.8f, 3.7f, 6.0f)});
    keyFrames.push_back({34.0f, glm::vec3(-11.6f, 4.8f, 4.5f), glm::vec3(-10.4f, 4.6f, 7.9f)});
    keyFrames.push_back({37.0f, glm::vec3(-8.1f, 3.9f, 10.5f), glm::vec3(-10.4f, 3.3f, 13.6f)});
    keyFrames.push_back({39.0f, glm::vec3(-9.0f, 3.9f, 15.1f), glm::vec3(-13.54f, 2.44f, 21.19f)});
    keyFrames.push_back({43.0f, glm::vec3(-12.82f, 3.9f, 16.35f), glm::vec3(-12.82f, 3.9f, 21.23f)});
    keyFrames.push_back({47.0f, glm::vec3(-16.32f, 4.5f, 14.31f), glm::vec3(-12.82f, 3.9f, 20.23f)});
    KeyFrameAnimation::SetKeyFrames(keyFrames);
    bool useKeyFrames = true;

    oitFBO = std::make_shared<Graphics::Framebuffer>(Graphics::FramebufferSpecification{p_Window->GetFramebufferSize().first, p_Window->GetFramebufferSize().second, {GL_RGBA16F, GL_R16F, GL_DEPTH24_STENCIL8}});

    uint32_t previousFramerates[1000] = {};
    uint32_t index = 0;
    std::chrono::steady_clock::time_point lastFrame = std::chrono::steady_clock::now();
    double deltaTime = 0;
    double lightAngle = 0.0;
    while (!p_Window->ShouldClose())
    {
        p_Window->PollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        camera.OnUpdate(p_Window, deltaTime);
        glm::mat4 view = useKeyFrames ? KeyFrameAnimation::getAnimatedViewMatrix(glfwGetTime()) : camera.GetCamera().getViewMatrix();

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        worldRenderer.NewFrame();

        lightAngle += deltaTime;
        glm::vec3 lightPosition = glm::vec3(sinf(glm::radians(lightAngle * 30.f)) * 5.f, 5.0f, 2.0f);
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        whiteTexture->BindToSlot(0);

        shader->Bind();
        shader->SetMat4Uniform("u_Transform", glm::scale(glm::translate(glm::mat4(1.0f), lights[pointLightCount - 1].position), glm::vec3(0.1f)));
        shader->SetMat4Uniform("u_ViewProjection", camera.GetCamera().getProjectionMatrix() * view);
        shader->SetFloat3Uniform("u_Color", lights[pointLightCount - 1].diffuse);
        //shader->SetMat4Uniform("u_ViewProjection", orbitCamera.GetViewProjectionMatrix());
        shader->SetInt("u_Texture", 0);
        cubeVao->Bind();
        glDrawElements(GL_TRIANGLES, cubeIbo->GetCount(), GL_UNSIGNED_INT, nullptr);
        cubeVao->Unbind();
        shader->Unbind();

        if (!useKeyFrames)
        {
            ImGui::Begin("Light");
            ImGui::DragFloat3("Position", glm::value_ptr(lights[pointLightCount - 1].position), 0.01f);
            ImGui::DragFloat3("Ambient", glm::value_ptr(lights[pointLightCount - 1].ambient), 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat3("Diffuse", glm::value_ptr(lights[pointLightCount - 1].diffuse), 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat3("Specular", glm::value_ptr(lights[pointLightCount - 1].specular), 0.01f, 0.0f, 1.0f);
            ImGui::End();
        }

        modelShader->Bind();
        modelShader->SetInt("u_PointLightCount", pointLightCount);
        for (int i = 0; i < pointLightCount; i++)
        {
            modelShader->SetFloat3Uniform(std::format("u_PointLights[{}].position", i), lights[i].position);
            modelShader->SetFloat3Uniform(std::format("u_PointLights[{}].ambient", i), lights[i].ambient);
            modelShader->SetFloat3Uniform(std::format("u_PointLights[{}].diffuse", i), lights[i].diffuse);
            modelShader->SetFloat3Uniform(std::format("u_PointLights[{}].specular", i), lights[i].specular);
            modelShader->SetFloat(std::format("u_PointLights[{}].constant", i), lights[i].constant);
            modelShader->SetFloat(std::format("u_PointLights[{}].linear", i), lights[i].linear);
            modelShader->SetFloat(std::format("u_PointLights[{}].quadratic", i), lights[i].quadratic);
        }
        modelShader->SetFloat3Uniform("u_DirLight.direction", dirLight.direction);
        modelShader->SetFloat3Uniform("u_DirLight.ambient", dirLight.ambient);
        modelShader->SetFloat3Uniform("u_DirLight.diffuse", dirLight.diffuse);
        modelShader->SetFloat3Uniform("u_DirLight.specular", dirLight.specular);
        modelShader->SetFloat3Uniform("u_CameraPosition", camera.GetCamera().getPosition());
        modelShader->Unbind();

        oitAccumShader->Bind();
        oitAccumShader->SetInt("u_PointLightCount", pointLightCount);
        for (int i = 0; i < pointLightCount; i++)
        {
            oitAccumShader->SetFloat3Uniform(std::format("u_PointLights[{}].position", i), lights[i].position);
            oitAccumShader->SetFloat3Uniform(std::format("u_PointLights[{}].ambient", i), lights[i].ambient);
            oitAccumShader->SetFloat3Uniform(std::format("u_PointLights[{}].diffuse", i), lights[i].diffuse);
            oitAccumShader->SetFloat3Uniform(std::format("u_PointLights[{}].specular", i), lights[i].specular);
            oitAccumShader->SetFloat(std::format("u_PointLights[{}].constant", i), lights[i].constant);
            oitAccumShader->SetFloat(std::format("u_PointLights[{}].linear", i), lights[i].linear);
            oitAccumShader->SetFloat(std::format("u_PointLights[{}].quadratic", i), lights[i].quadratic);
        }
        oitAccumShader->SetFloat3Uniform("u_DirLight.direction", dirLight.direction);
        oitAccumShader->SetFloat3Uniform("u_DirLight.ambient", dirLight.ambient);
        oitAccumShader->SetFloat3Uniform("u_DirLight.diffuse", dirLight.diffuse);
        oitAccumShader->SetFloat3Uniform("u_DirLight.specular", dirLight.specular);
        oitAccumShader->SetFloat3Uniform("u_CameraPosition", camera.GetCamera().getPosition());
        oitAccumShader->Unbind();

        //worldRenderer.Render(map, modelShader, glm::mat4(1.0f), view, camera.GetCamera().getProjectionMatrix());


        //glm::mat4 portalGunCameraTransform = glm::inverse(camera.GetCamera().getViewMatrix());
        //glm::mat4 portalGunTransform = glm::translate(glm::mat4(1.0f), portalGunPosition);
        //portalGunTransform = glm::rotate(portalGunTransform, glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
        //ModelRenderer::Render(portalGun, modelShader, portalGunCameraTransform * portalGunTransform, camera.GetCamera().getViewMatrix(), camera.GetCamera().getProjectionMatrix());

        if (!useKeyFrames)
        {
            ImGui::Begin("Blue Portal");
            ImGui::DragFloat3("Position", glm::value_ptr(bluePortal2Position), 0.01f);
            ImGui::DragFloat("Rotation", &bluePortal2Rotation, 0.01f);
            ImGui::End();

            ImGui::Begin("Orange Portal");
            ImGui::DragFloat3("Position", glm::value_ptr(orangePortal2Position), 0.01f);
            ImGui::DragFloat("Rotation", &orangePortal2Rotation, 0.01f);
            ImGui::End();

            ImGui::Begin("Cube");
            ImGui::DragFloat3("Position", glm::value_ptr(cubePosition), 0.01f);
            ImGui::DragFloat("Rotation", &cubeRotation, 0.01f);
            ImGui::DragFloat("Scale", &cubeScale, 0.01f);
            ImGui::End();
        }

        std::vector<PortalRenderData> scene;
        PortalRenderData mapData{map, glm::mat4(1.0f)};
        scene.push_back(mapData);
        PortalRenderData cubeData{cube, glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), cubePosition), cubeRotation, glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(cubeScale))};
        scene.push_back(cubeData);
        PortalRenderer::SetScene(scene, modelShader);

        for (auto& data : scene)
        {
            worldRenderer.Render(data.model, modelShader, data.transform, view, camera.GetCamera().getProjectionMatrix());
        }

        glm::mat4 bluePortalTransform = glm::rotate(glm::translate(glm::mat4(1.0f), bluePortalPosition), bluePortalRotation, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 orangePortalTransform = glm::rotate(glm::translate(glm::mat4(1.0f), orangePortalPosition), orangePortalRotation, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 bluePortal2Transform = glm::rotate(glm::translate(glm::mat4(1.0f), bluePortal2Position), bluePortal2Rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 orangePortal2Transform = glm::rotate(glm::translate(glm::mat4(1.0f), orangePortal2Position), orangePortal2Rotation, glm::vec3(0.0f, 1.0f, 0.0f));

        if (glfwGetTime() < 37.0f) PortalRenderer::Render(bluePortalTransform, orangePortalTransform, view, camera.GetCamera().getProjectionMatrix(), oitFBO, oitAccumShader, oitCompositeShader);
        else PortalRenderer::Render(bluePortal2Transform, orangePortal2Transform, view, camera.GetCamera().getProjectionMatrix(), oitFBO, oitAccumShader, oitCompositeShader);

        worldRenderer.EndFrame(oitFBO, oitAccumShader, oitCompositeShader);

        if (!useKeyFrames)
        {
            ImGui::Begin("Camera Debug");
            ImGui::DragFloat3("Position", glm::value_ptr(camera.GetCamera().getPosition()));
            ImGui::DragFloat3("Rotation", glm::value_ptr(camera.GetCamera().GetRotation()));
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        p_Window->SwapBuffers();

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - lastFrame;
        deltaTime = elapsed_seconds.count();
        lastFrame = std::chrono::steady_clock::now();
        previousFramerates[index] = (1.0 / elapsed_seconds.count());
        uint32_t framerate = 0;
        uint32_t lowFramerates[10];
        uint32_t lowIndex = 0;
        uint32_t lastIndex = 0;
        for (uint32_t i = 0; i < 1000; i++)
        {
            if (previousFramerates[i] == 0) break;
            framerate += previousFramerates[i];
            if (lowIndex < 10)
            {
                lowFramerates[lowIndex] = previousFramerates[i];
                lowIndex++;
            }
            else
            {
                uint32_t jmax = 0;
                for (uint32_t j = 1; j < 10; j++)
                {
                    if (lowFramerates[jmax] < previousFramerates[j]) jmax = j;
                }
                lowFramerates[jmax] = previousFramerates[i];
            }
            lastIndex = i;
        }
        index++;
        if (index >= 1000) index = 0;
        if (lastIndex != 0) framerate = framerate / lastIndex;
        uint32_t lowFramerate = 0;
        for (uint32_t i = 0; i < 10; i++)
        {
            lowFramerate += lowFramerates[i];
        }
        lowFramerate /= 10;
        p_Window->SetTitle(std::format("OpenGL - FPS: {} - 1%Low: {}", framerate, lowFramerate));
    }

    PortalRenderer::Shutdown();
    worldRenderer.ShutDown();

    oitAccumShader.reset();
    oitCompositeShader.reset();
    oitFBO.reset();
    cubeVao.reset();
    cubeIbo.reset();
    cubeVbo.reset();
    portalGun.reset();
    map.reset();
    crateTexture.reset();
    whiteTexture.reset();
    shader.reset();
    modelShader.reset();
    WindowManager::Get().DestroyMainWindow();

    Utils::Terminate();

    return 0;
}
