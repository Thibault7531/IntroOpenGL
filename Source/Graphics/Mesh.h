#pragma once
#include <memory>

#include "Buffers.h"
#include "Texture.h"
#include "VertexArray.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

class Mesh
{
public:
    Mesh(aiMesh* mesh, const aiScene* scene);
    ~Mesh() = default;

    std::shared_ptr<Graphics::VertexArray> GetVertexArray() const { return m_VertexArray; }
    uint32_t GetMaterialIndex() const { return m_MaterialIndex; }

private:
    std::shared_ptr<Graphics::VertexArray> m_VertexArray;
    uint32_t m_MaterialIndex;
};
