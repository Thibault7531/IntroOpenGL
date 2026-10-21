#include "Framebuffer.h"

#include <iostream>

namespace Graphics
{
    Framebuffer::Framebuffer(const FramebufferSpecification& spec)
    {
        for (uint32_t i = 0; i < spec.AttachmentFormats.size(); i++)
        {
            if (IsDepthFormat(spec.AttachmentFormats[i]))
            {
                m_DepthAttachmentFormat = spec.AttachmentFormats[i];
            }
            else
            {
                m_ColorAttachmentFormats.push_back(spec.AttachmentFormats[i]);
            }
        }
        m_Width = spec.Width;
        m_Height = spec.Height;

        glGenFramebuffers(1, &m_FramebufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

        std::vector<GLenum> drawBuffers;
        for (uint32_t i = 0; i < m_ColorAttachmentFormats.size(); i++)
        {
            m_ColorAttachments.push_back(std::make_shared<Texture>(m_Width, m_Height, m_ColorAttachmentFormats[i]));
            drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
            m_ColorAttachments[i]->BindToFramebufferSlot(GL_COLOR_ATTACHMENT0 + i);
        }
        if (m_DepthAttachmentFormat != GL_NONE)
        {
            m_DepthAttachment = std::make_shared<Texture>(m_Width, m_Height, m_DepthAttachmentFormat);
            m_DepthAttachment->BindToFramebufferSlot(IsDepthStencilFormat(m_DepthAttachmentFormat) ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT);
        }

        glDrawBuffers(drawBuffers.size(), drawBuffers.data());

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Framebuffer incomplete: " << status << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    Framebuffer::~Framebuffer()
    {
        for (auto& attachment : m_ColorAttachments)
        {
            attachment.reset();
        }
        m_DepthAttachment.reset();
        glDeleteFramebuffers(1, &m_FramebufferID);
    }

    void Framebuffer::Resize(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;

        for (auto& attachment : m_ColorAttachments)
        {
            attachment.reset();
        }
        m_ColorAttachments.clear();
        m_DepthAttachment.reset();
        glDeleteFramebuffers(1, &m_FramebufferID);

        glGenFramebuffers(1, &m_FramebufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

        std::vector<GLenum> drawBuffers;
        for (uint32_t i = 0; i < m_ColorAttachmentFormats.size(); i++)
        {
            m_ColorAttachments.push_back(std::make_shared<Texture>(m_Width, m_Height, m_ColorAttachmentFormats[i]));
            drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
            m_ColorAttachments[i]->BindToFramebufferSlot(GL_COLOR_ATTACHMENT0 + i);
        }
        if (m_DepthAttachmentFormat != GL_NONE)
        {
            m_DepthAttachment = std::make_shared<Texture>(m_Width, m_Height, m_DepthAttachmentFormat);
            m_DepthAttachment->BindToFramebufferSlot(IsDepthStencilFormat(m_DepthAttachmentFormat) ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT);
        }

        glDrawBuffers(drawBuffers.size(), drawBuffers.data());

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Framebuffer incomplete: " << status << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
    }

    void Framebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    bool Framebuffer::IsDepthStencilFormat(GLint format)
    {
        switch (format)
        {
        case GL_DEPTH24_STENCIL8: return true;
        default: return false;
        }
    }

    bool Framebuffer::IsDepthFormat(GLint format)
    {
        switch (format)
        {
        case GL_DEPTH24_STENCIL8: return true;
        default: return false;
        }
    }
}
