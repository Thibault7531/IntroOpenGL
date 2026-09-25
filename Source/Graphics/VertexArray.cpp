#include "VertexArray.h"

#include "glad/gl.h"

namespace Graphics
{
    VertexArray::VertexArray()
    {
        glCreateVertexArrays(1, &m_ArrayID);
    }

    VertexArray::~VertexArray()
    {
        
        glDeleteVertexArrays(1, &m_ArrayID);
    }

    void VertexArray::SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo)
    {
        m_VertexBuffer = vbo;
        glBindVertexArray(m_ArrayID);
        m_VertexBuffer->Bind();

        VertexLayout layout = m_VertexBuffer->GetLayout();
        uint32_t currentIndex = 0;
        for (const auto& element : layout.Layout)
        {
            glVertexAttribPointer(currentIndex, element.GetComponentCount(), GetOpenGLDataType(element.Type), GL_FALSE, layout.Stride, (void*)element.Offset);
            glEnableVertexAttribArray(currentIndex);
            currentIndex++;
        }

        glBindVertexArray(0);
    }

    void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ibo)
    {
        m_IndexBuffer = ibo;
        glBindVertexArray(m_ArrayID);
        m_IndexBuffer->Bind();
        glBindVertexArray(0);
    }

    void VertexArray::Bind()
    {
        glBindVertexArray(m_ArrayID);
    }

    void VertexArray::Unbind()
    {
        glBindVertexArray(0);
    }

    GLenum VertexArray::GetOpenGLDataType(VertexDataType type)
    {
        switch (type)
        {
        case VertexDataType::Float2: return  GL_FLOAT;
        case VertexDataType::Float3: return  GL_FLOAT;
        }
    }
}
