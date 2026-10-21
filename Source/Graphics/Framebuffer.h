#pragma once

#include <vector>

#include "Texture.h"
#include "glad/gl.h"

namespace Graphics
{
    struct FramebufferSpecification
    {
        uint32_t Width;
        uint32_t Height;

        std::vector<GLint> AttachmentFormats;
    };

    class Framebuffer
    {
    public:
        Framebuffer(const FramebufferSpecification& spec);
        ~Framebuffer();

        GLuint GetFramebufferID() const { return m_FramebufferID; }
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        std::shared_ptr<Graphics::Texture> GetColorAttachment(uint32_t index) const { return m_ColorAttachments[index]; }

        void Bind();
        void Unbind();

    public:
        static bool IsDepthStencilFormat(GLint format);
        static bool IsDepthFormat(GLint format);

    private:
        GLuint m_FramebufferID;
        uint32_t m_Width;
        uint32_t m_Height;

        std::vector<std::shared_ptr<Texture>> m_ColorAttachments;
        std::vector<GLuint> m_ColorAttachmentFormats;
        std::shared_ptr<Texture> m_DepthAttachment;
        GLuint m_DepthAttachmentFormat = GL_NONE;
    };
}