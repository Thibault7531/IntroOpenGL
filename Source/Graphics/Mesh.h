#pragma once
#include <memory>

#include "Buffers.h"
#include "Texture.h"
#include "VertexArray.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "glm/glm.hpp"

class Mesh
{
private:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 colorUV;
        glm::vec2 emissiveUV;
        glm::vec3 normal;
    };

    struct TriangleSortKey
    {
        uint32_t i0, i1, i2;
        float distanceSq;
    };

public:
    Mesh(glm::mat4 transform, aiMesh* mesh, const aiScene* scene);
    ~Mesh() = default;

    std::shared_ptr<Graphics::VertexArray> GetVertexArray() const { return m_VertexArray; }
    uint32_t GetMaterialIndex() const { return m_MaterialIndex; }
    const glm::mat4& GetTransform() const { return m_Transform; }

    void SortTransparentMesh(const glm::mat4 view);

private:
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
    std::shared_ptr<Graphics::VertexArray> m_VertexArray;
    uint32_t m_MaterialIndex;

    glm::mat4 m_Transform;
};
