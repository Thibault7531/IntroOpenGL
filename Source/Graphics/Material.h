#pragma once

#include <vector>

#include "Buffers.h"
#include "Texture.h"
#include "assimp/material.h"
#include "assimp/scene.h"
#include "glm/glm.hpp"

class Material
{
public:
    Material(std::filesystem::path path, aiMaterial* material, const aiScene* scene);

    void SetupRenderState() const;

    const glm::vec3& GetAmbientColor() const { return m_AmbientColor; }
    const std::vector<std::shared_ptr<Graphics::Texture>>& GetColorTextures() const { return m_ColorTextures; }
    std::shared_ptr<Graphics::Texture> GetEmissiveTexture() const { return m_EmissiveTexture; }
    const glm::vec3& GetSpecularColor() const { return m_SpecularColor; }
    float GetShininess() const { return m_Shininess; }
    const glm::vec4& GetBaseColor() const { return m_BaseColor; }
    float GetOpacity() const { return m_Opacity; }
    bool HasTransparency() const { return m_HasTransparency; }

private:
    glm::vec3 m_AmbientColor;
    std::vector<std::shared_ptr<Graphics::Texture>> m_ColorTextures;
    std::shared_ptr<Graphics::Texture> m_EmissiveTexture;
    glm::vec3 m_SpecularColor;
    float m_Shininess;

    glm::vec4 m_BaseColor{1.0f};
    float m_Opacity = 0.0f;
    bool m_HasTransparency = false;
    bool m_DoubleSided = false;
    bool m_Blend = false;
    GLenum m_BlendOperationSource = GL_SRC_ALPHA, m_BlendOperationDestination = GL_ONE_MINUS_SRC_ALPHA;
};
