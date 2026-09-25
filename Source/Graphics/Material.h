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

    const std::vector<std::shared_ptr<Graphics::Texture>>& GetColorTextures() const { return m_ColorTextures; }
    float GetOpacity() const { return m_Opacity; }
    glm::vec4 GetColor() const { return m_Color; }

private:
    std::vector<std::shared_ptr<Graphics::Texture>> m_ColorTextures;

    float m_Opacity;
    glm::vec4 m_Color;
    bool m_DoubleSided = false;
    bool m_Blend = false;
    GLenum m_BlendOperationSource = GL_SRC_ALPHA, m_BlendOperationDestination = GL_ONE_MINUS_SRC_ALPHA;
};
