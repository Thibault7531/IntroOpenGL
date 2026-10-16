#include "Material.h"

Material::Material(std::filesystem::path path, aiMaterial* material, const aiScene* scene)
{
    aiColor4D ambient;
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambient))
    {
        m_AmbientColor = glm::vec3(ambient.r, ambient.g, ambient.b);
    }
    else
    {
        m_AmbientColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    aiColor4D color;
    aiGetMaterialColor(material, AI_MATKEY_BASE_COLOR, &color);
    m_BaseColor = glm::vec4(color.r, color.g, color.b, color.a);
    m_Opacity = color.a;

    int textureCount = material->GetTextureCount(aiTextureType_BASE_COLOR);
    if (textureCount > 0)
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
    }
    else
    {
        for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
        {
            aiString texturePath;
            material->GetTexture(aiTextureType_DIFFUSE, i, &texturePath);

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
    }

    if (m_ColorTextures.size() == 0)
    {
        uint8_t whiteTextureData[] = {static_cast<uint8_t>(color.r * 255), static_cast<uint8_t>(color.g * 255), static_cast<uint8_t>(color.b * 255), static_cast<uint8_t>(color.a * 255)};
        m_ColorTextures.push_back(std::make_shared<Graphics::Texture>(1, 1));
        m_ColorTextures[0]->SetData(whiteTextureData, GL_RGBA);
    }
    if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0)
    {
        aiString texturePath;
        material->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath);

        const aiTexture* texture = scene->GetEmbeddedTexture(texturePath.C_Str());
        if (texture)
        {
            m_EmissiveTexture = std::make_shared<Graphics::Texture>(texture->mWidth, texture->mHeight);
            m_EmissiveTexture->SetData(texture->pcData, GL_BGRA);
        }
        else
        {
            m_EmissiveTexture = std::make_shared<Graphics::Texture>(path / texturePath.C_Str());
        }
    }
    else
    {
        uint8_t blackTextureData[] = {0, 0, 0, 0};
        m_EmissiveTexture = std::make_shared<Graphics::Texture>(1, 1);
        m_EmissiveTexture->SetData(blackTextureData, GL_RGBA);
    }

    m_HasTransparency = m_Opacity < 1.0f || m_ColorTextures[0]->HasTransparency();

    aiColor4D specular;
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, specular))
    {
        m_SpecularColor = glm::vec3(specular.r, specular.g, specular.b);
    }
    else
    {
        m_SpecularColor = glm::vec3(1.0f, 1.0f, 1.0f);
    }
    if (!material->Get(AI_MATKEY_SHININESS, m_Shininess))
    {
        m_Shininess = 5.f;
    }

    material->Get(AI_MATKEY_TWOSIDED, m_DoubleSided);
    float blendMode;
    material->Get(AI_MATKEY_BLEND_FUNC, blendMode);
    switch (static_cast<aiBlendMode>(blendMode)) {
    case aiBlendMode_Default:
        m_Blend = m_HasTransparency;
        m_BlendOperationSource = GL_SRC_ALPHA;
        m_BlendOperationDestination = GL_ONE_MINUS_SRC_ALPHA;
        break;

    case aiBlendMode_Additive:
        m_Blend = m_HasTransparency;
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

        //glDepthMask(GL_FALSE);
        //glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_BLEND);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }
}
