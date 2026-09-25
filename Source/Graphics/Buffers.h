#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "glad/gl.h"

namespace Graphics
{
    enum class VertexDataType
    {
        Float2,
        Float3
    };

    struct VertexData
    {
        std::string Name;
        VertexDataType Type;
        uint32_t Offset;

        uint32_t GetStride() const
        {
            switch (Type)
            {
            case VertexDataType::Float2: return 2 * sizeof(float);
            case VertexDataType::Float3: return 3 * sizeof(float);
            }
            return 0;
        }
        uint32_t GetComponentCount() const
        {
            switch (Type)
            {
            case VertexDataType::Float2: return 2;
            case VertexDataType::Float3: return 3;
            }
            return 0;
        }

        VertexData(const std::string& name, VertexDataType type) : Name(name), Type(type), Offset(0) {}
    };

    struct VertexLayout
    {
        std::vector<VertexData> Layout;
        uint32_t Stride;

        void CalculateStrideAndSize()
        {
            uint32_t currentOffset = 0;
            for (VertexData& data : Layout)
            {
                data.Offset = currentOffset;
                currentOffset += data.GetStride();
            }
            Stride = currentOffset;
        }

        VertexLayout(std::initializer_list<VertexData> layout) : Layout(layout) { CalculateStrideAndSize(); }
    };

    class VertexBuffer
    {
    public:
        VertexBuffer(uint32_t size);
        VertexBuffer(void* data, uint32_t size);
        ~VertexBuffer();

        void SetLayout(const VertexLayout& layout) { m_Layout = layout; }
        VertexLayout GetLayout() { return m_Layout; }

        void Bind();
        void Unbind();

    private:
        uint32_t m_BufferID;
        uint32_t m_Size;
        VertexLayout m_Layout;
    };

    class IndexBuffer
    {
    public:
        IndexBuffer(uint32_t count);
        IndexBuffer(uint32_t* data, uint32_t count);
        ~IndexBuffer();

        uint32_t GetCount() { return m_Count; }

        void Bind();
        void Unbind();

    private:
        uint32_t m_BufferID;
        uint32_t m_Count;
    };
}
