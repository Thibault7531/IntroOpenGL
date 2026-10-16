#pragma once
#include <filesystem>

#include "glad/gl.h"

namespace Graphics
{
    class Texture
    {
    public:
        Texture(std::filesystem::path texturePath);
        Texture(uint32_t width, uint32_t height);

        void SetData(void* data, GLenum format);
        uint32_t GetChannelCount() const { return m_ChannelCount; }
        bool HasTransparency() const { return m_HasTransparency; }

        void Bind();
        void BindToSlot(uint32_t slot = 0);
        void Unbind();

    private:
        bool DetectTransparency(const uint8_t* data, size_t total_bytes);

    private:
        uint32_t m_TextureId;
        uint32_t m_Width, m_Height;

        uint32_t m_ChannelCount = 0;
        bool m_HasTransparency = false;
    };
}
