#include "Material.h"

Material::Material(std::filesystem::path path, aiMaterial* material, const aiScene* scene)
{
    for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_BASE_COLOR); i++)
    {
        aiString texturePath;
        material->GetTexture(aiTextureType_BASE_COLOR, i, &texturePath);

        const aiTexture* texture = scene->GetEmbeddedTexture(texturePath.C_Str());
        if (texture)
        {
            m_ColorTextures.push_back(std::make_shared<Graphics::Texture>(texture->mWidth, texture->mHeight));
            m_ColorTextures[i]->SetData(texture->pcData, GL_BGRA);
        }
        else
        {
            m_ColorTextures.push_back(std::make_shared<Graphics::Texture>(path / texturePath.C_Str()));
        }
    }
    if (m_ColorTextures.size() == 0)
    {
        uint8_t whiteTextureData[] = {255, 255, 255, 255};
        m_ColorTextures.push_back(std::make_shared<Graphics::Texture>(1, 1));
        m_ColorTextures[0]->SetData(whiteTextureData, GL_RGBA);
    }

    aiColor4D color;
    aiGetMaterialColor(material, AI_MATKEY_BASE_COLOR, &color);
    material->Get(AI_MATKEY_OPACITY, m_Opacity);
    m_Color = glm::vec4(color.r, color.g, color.b, color.a);

    material->Get(AI_MATKEY_TWOSIDED, m_DoubleSided);
    aiBlendMode blendMode;
    material->Get(AI_MATKEY_BLEND_FUNC, blendMode);
    switch (blendMode) {
    case aiBlendMode_Default:
        m_Blend = m_Opacity != 1.0f;
        m_BlendOperationSource = GL_SRC_ALPHA;
        m_BlendOperationDestination = GL_ONE_MINUS_SRC_ALPHA;
        break;

    case aiBlendMode_Additive:
        m_Blend = m_Opacity != 1.0f;
        m_BlendOperationSource = GL_SRC_ALPHA;
        m_BlendOperationDestination = GL_ONE;
        break;

    default:
        m_Blend = false;
        break;
    }
}

void Material::SetupRenderState() const
{
    for (uint32_t i = 0; i < m_ColorTextures.size(); i++)
    {
        m_ColorTextures[i]->BindToSlot(i);
    }

    if (m_DoubleSided)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
    }

    if (m_Blend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(m_BlendOperationSource, m_BlendOperationDestination);

        glDepthMask(GL_FALSE);
    }
    else
    {
        glDisable(GL_BLEND);

        glDepthMask(GL_TRUE);
    }
}
