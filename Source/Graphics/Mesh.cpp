#include "Mesh.h"

Mesh::Mesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);
        if (mesh->mTextureCoords[0])
        {
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }

        if (mesh->mNormals)
        {
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);
        }
        else
        {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
    }

    m_VertexArray = std::make_shared<Graphics::VertexArray>();
    std::shared_ptr<Graphics::VertexBuffer> vbo = std::make_shared<Graphics::VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
    vbo->SetLayout({
        {"a_Position", Graphics::VertexDataType::Float3 },
        {"a_TexCoord", Graphics::VertexDataType::Float2 },
        {"a_Normal", Graphics::VertexDataType::Float3 }
    });
    m_VertexArray->SetVertexBuffer(vbo);
    std::shared_ptr<Graphics::IndexBuffer> ibo = std::make_shared<Graphics::IndexBuffer>(indices.data(), indices.size());
    m_VertexArray->SetIndexBuffer(ibo);

    m_MaterialIndex = mesh->mMaterialIndex;
}
