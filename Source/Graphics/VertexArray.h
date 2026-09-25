#pragma once

#include <memory>

#include "Buffers.h"

namespace Graphics
{
    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo);
        void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ibo);

        uint32_t GetIndexCount() const { return m_IndexBuffer->GetCount(); }

        void Bind();
        void Unbind();

    public:
        static GLenum GetOpenGLDataType(VertexDataType type);

    private:
        uint32_t m_ArrayID;

        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
    };
}
