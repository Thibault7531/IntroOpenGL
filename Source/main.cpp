#include <chrono>
#include <iostream>

#include "Camera/CameraController.h"
#include "Camera/OrbitCamera.h"
#include "Camera/PerspectiveCamera.h"
#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "glm/ext/matrix_transform.hpp"
#include "Graphics/Buffers.h"
#include "Graphics/Model.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexArray.h"
#include "Windowing/Utils.h"
#include "Windowing/Window.h"
#include "Windowing/WindowManager.h"

static PerspectiveCameraController camera{16.f/9.f, glm::radians(60.f)};
static OrbitCamera orbitCamera{glm::radians(60.0f), 1920.0f/1080.0f, 0.001f, 1000.0f};

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
}

int main()
{
    Utils::Init();

    Window* p_Window = WindowManager::Get().CreateMainWindow({"OpenGL", 1920, 1080});
    p_Window->SetKeyCallback(OnKeyPressedEvent);
    p_Window->SetCursorPosCallback(OnCursorPosEvent);
    p_Window->SetMouseScrollCallback(OnMouseScrollEvent);
    p_Window->SetFramebufferResizeCallback(OnFramebufferResized);

    std::shared_ptr<Graphics::Shader> shader = std::make_shared<Graphics::Shader>(std::filesystem::path("assets") / "triangleVert.glsl", std::filesystem::path("assets") / "triangleFrag.glsl");

    camera.GetCamera().setPosition({0.f, 0.f, 3.f});
    orbitCamera.SetDistance(5.0f);

    uint8_t whiteTextureData[] = {255, 255, 255, 255};
    std::shared_ptr<Graphics::Texture> whiteTexture = std::make_shared<Graphics::Texture>(1, 1);
    whiteTexture->SetData(&whiteTextureData, GL_RGBA);

    std::shared_ptr<Graphics::Texture> crateTexture = std::make_shared<Graphics::Texture>("assets/wooden_crate.jpg");

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.f, 0.f, 0.f, 0.f, 0.f,
         0.5f, -0.5f, 0.0f, 0.f, 1.f, 0.f, 1.f, 0.f,
         -0.5f,  0.5f, 0.0f, 0.f, 0.f, 1.f, 0.f, 1.f,
        0.5f,  0.5f, 0.0f, 1.f, 1.f, 1.f, 1.f, 1.f
    };
    uint32_t indices[] = {
        0, 1, 2,
        3, 2, 1
    };
    std::shared_ptr<Graphics::VertexBuffer> vbo = std::make_shared<Graphics::VertexBuffer>(vertices, sizeof(vertices));
    vbo->SetLayout({
        {"a_Position", Graphics::VertexDataType::Float3},
        {"a_Color", Graphics::VertexDataType::Float3},
        {"a_TexCoord", Graphics::VertexDataType::Float2}
    });
    std::shared_ptr<Graphics::IndexBuffer> ibo = std::make_shared<Graphics::IndexBuffer>(indices, sizeof(indices) / sizeof(uint32_t));

    std::shared_ptr<Graphics::VertexArray> vao = std::make_shared<Graphics::VertexArray>();
    vao->SetVertexBuffer(vbo);
    vao->SetIndexBuffer(ibo);

    float cubeVertices[] = {
        // Front face
        -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.f, 0.f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.f, 0.f,
        -0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.f, 1.f,
        0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.f, 1.f,

        // Right face
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.f, 0.f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.f, 0.f,
        0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.f, 1.f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.f, 1.f,

        // Back face
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.f, 0.f,
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.f, 0.f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.f, 1.f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.f, 1.f,

        // Left face
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.f, 0.f,
        -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.f, 0.f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.f, 1.f,
        -0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.f, 1.f,

        // Bottom face
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

        // Top face
        -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
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
            {"a_Color", Graphics::VertexDataType::Float3},
            {"a_TexCoord", Graphics::VertexDataType::Float2}
    });
    std::shared_ptr<Graphics::IndexBuffer> cubeIbo = std::make_shared<Graphics::IndexBuffer>(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t));

    std::shared_ptr<Graphics::VertexArray> cubeVao = std::make_shared<Graphics::VertexArray>();
    cubeVao->SetVertexBuffer(cubeVbo);
    cubeVao->SetIndexBuffer(cubeIbo);

    std::shared_ptr<Graphics::Shader> modelShader = std::make_shared<Graphics::Shader>(std::filesystem::path("assets/ModelVert.glsl"), "assets/ModelFrag.glsl");

    std::shared_ptr<Model> portalGun = std::make_shared<Model>("assets/PortalGun/scene.gltf");
    std::shared_ptr<Model> map = std::make_shared<Model>("assets/Portal Map/scene.gltf");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    uint32_t previousFramerates[1000] = {};
    uint32_t index = 0;
    std::chrono::steady_clock::time_point lastFrame = std::chrono::steady_clock::now();
    double deltaTime = 0;
    double cubeRotation = 0.0;
    while (!p_Window->ShouldClose())
    {
        p_Window->PollEvents();
        camera.OnUpdate(p_Window, deltaTime);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        whiteTexture->BindToSlot(0);
        crateTexture->BindToSlot(1);

        shader->Bind();
        shader->SetMat4Uniform("u_Transform", glm::translate(glm::mat4(1.0f), glm::vec3(-0.25f, 2.25f, 0.0f)));
        shader->SetMat4Uniform("u_ViewProjection", camera.GetCamera().getViewProjectionMatrix());
        //shader->SetMat4Uniform("u_ViewProjection", orbitCamera.GetViewProjectionMatrix());
        shader->SetInt("u_Texture", 0);
        vao->Bind();
        glDrawElements(GL_TRIANGLES, ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
        vao->Unbind();
        shader->Unbind();

        shader->Bind();
        shader->SetMat4Uniform("u_Transform", glm::translate(glm::mat4(1.0f), glm::vec3(-0.0f, 2.0f, 0.25f)));
        shader->SetInt("u_Texture", 1);
        vao->Bind();
        glDrawElements(GL_TRIANGLES, ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
        vao->Unbind();
        shader->Unbind();

        crateTexture->BindToSlot(0);

        cubeRotation += deltaTime;

        shader->Bind();
        shader->SetMat4Uniform("u_Transform", glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), static_cast<float>(cubeRotation), glm::vec3(0.0f, 1.0f, 0.0f)));
        shader->SetMat4Uniform("u_ViewProjection", camera.GetCamera().getViewProjectionMatrix());
        //shader->SetMat4Uniform("u_ViewProjection", orbitCamera.GetViewProjectionMatrix());
        shader->SetInt("u_Texture", 0);
        cubeVao->Bind();
        glDrawElements(GL_TRIANGLES, cubeIbo->GetCount(), GL_UNSIGNED_INT, nullptr);
        cubeVao->Unbind();
        shader->Unbind();

        glm::vec3 lightPosition = glm::vec3(sinf(glm::radians(cubeRotation * 30.f)) * 5.f, 5.0f, 2.0f);
        whiteTexture->BindToSlot(0);

        shader->Bind();
        shader->SetMat4Uniform("u_Transform", glm::translate(glm::mat4(1.0f), lightPosition));
        shader->SetMat4Uniform("u_ViewProjection", camera.GetCamera().getViewProjectionMatrix());
        //shader->SetMat4Uniform("u_ViewProjection", orbitCamera.GetViewProjectionMatrix());
        shader->SetInt("u_Texture", 0);
        cubeVao->Bind();
        glDrawElements(GL_TRIANGLES, cubeIbo->GetCount(), GL_UNSIGNED_INT, nullptr);
        cubeVao->Unbind();
        shader->Unbind();

        modelShader->Bind();
        modelShader->SetFloat3Uniform("u_LightPosition", lightPosition);
        modelShader->Unbind();

        glm::mat4 transform = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.0f)), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(3.0f));
        portalGun->Render(modelShader, transform, camera.GetCamera().getViewProjectionMatrix());

        glm::mat4 mapTransform = 1.0f;
        map->Render(modelShader, mapTransform, camera.GetCamera().getViewProjectionMatrix());

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

    vao.reset();
    ibo.reset();
    vbo.reset();
    cubeVao.reset();
    cubeIbo.reset();
    cubeVbo.reset();
    portalGun.reset();
    crateTexture.reset();
    whiteTexture.reset();
    shader.reset();
    modelShader.reset();
    WindowManager::Get().DestroyMainWindow();

    Utils::Terminate();

    return 0;
}
