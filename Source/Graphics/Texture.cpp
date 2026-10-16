#include "Texture.h"

#include <immintrin.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glad/gl.h"

namespace Graphics
{
    Texture::Texture(std::filesystem::path texturePath)
    {
        stbi_set_flip_vertically_on_load(true);
        int32_t width, height, channel;
        uint8_t* textureData = stbi_load(texturePath.string().c_str(), &width, &height, &channel, STBI_rgb_alpha);
        GLenum texFormat = GL_RGBA;

        m_Width = width;
        m_Height = height;
        m_ChannelCount = 4;

        m_HasTransparency = DetectTransparency(textureData, width * height * m_ChannelCount);

        glGenTextures(1, &m_TextureId);
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, texFormat, m_Width, m_Height, 0, texFormat, GL_UNSIGNED_BYTE, textureData);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(textureData);
    }

    Texture::Texture(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
        m_ChannelCount = 4;

        glGenTextures(1, &m_TextureId);
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::SetData(void* data, GLenum format)
    {
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Bind()
    {
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
    }

    void Texture::BindToSlot(uint32_t slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
    }

    void Texture::Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    bool Texture::DetectTransparency(const uint8_t* data, size_t total_bytes)
    {
        __m256i v_255 = _mm256_set1_epi8(static_cast<char>(255));

        size_t i = 0;
        for (; i + 31 < total_bytes; i += 32)
        {
            __m256i pixels = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
            __m256i cmp = _mm256_cmpeq_epi8(pixels, v_255);
            uint32_t mask = _mm256_movemask_epi8(cmp);

            if (mask != 0xFFFFFFFF)
            {
                uint32_t alpha_mask = 0x88888888;
                uint32_t mismatches = ~mask;
                if (mismatches & alpha_mask)
                {
                    return true;
                }
            }
        }

        for (; i + 3 < total_bytes; i += 4)
        {
            if (data[i+3] < 255) return true;
        }

        return false;
    }
}
