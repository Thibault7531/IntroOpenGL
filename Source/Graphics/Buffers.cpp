#include "Buffers.h"

#include "glad/gl.h"

namespace Graphics
{
    VertexBuffer::VertexBuffer(uint32_t size)
        : m_Size(size), m_Layout{}
    {
        glCreateBuffers(1, &m_BufferID);
        glNamedBufferData(m_BufferID, size, nullptr, GL_STATIC_DRAW);
    }

    VertexBuffer::VertexBuffer(void* data, uint32_t size)
        : m_Size(size), m_Layout{}
    {
        glCreateBuffers(1, &m_BufferID);
        glNamedBufferData(m_BufferID, size, data, GL_STATIC_DRAW);
    }

    VertexBuffer::~VertexBuffer()
    {
        glDeleteBuffers(1, &m_BufferID);
    }

    void VertexBuffer::Bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    }

    void VertexBuffer::Unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    IndexBuffer::IndexBuffer(uint32_t count)
        : m_Count(count)
    {
        glCreateBuffers(1, &m_BufferID);
        glNamedBufferData(m_BufferID, count * sizeof(uint32_t), nullptr, GL_STATIC_DRAW);
    }

    IndexBuffer::IndexBuffer(uint32_t* data, uint32_t count)
        : m_Count(count)
    {
        glCreateBuffers(1, &m_BufferID);
        glNamedBufferData(m_BufferID, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
    }

    IndexBuffer::~IndexBuffer()
    {
        glDeleteBuffers(1, &m_BufferID);
    }

    void IndexBuffer::SetData(uint32_t* data, uint32_t count)
    {
        glNamedBufferData(m_BufferID, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
    }

    void IndexBuffer::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
    }

    void IndexBuffer::Unbind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}
