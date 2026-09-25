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

        void Bind();
        void BindToSlot(uint32_t slot = 0);
        void Unbind();

    private:
        uint32_t m_TextureId;
        uint32_t m_Width, m_Height;
    };
}
