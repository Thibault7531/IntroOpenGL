#include "Mesh.h"
#include "Mesh.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/io.hpp"

Mesh::Mesh(glm::mat4 transform, aiMesh* mesh, const aiScene* scene)
    : m_Transform(transform)
{
    m_MaterialIndex = mesh->mMaterialIndex;
    aiMaterial* material = scene->mMaterials[m_MaterialIndex];
    int colorChannel = 0;
    int emissionChannel = 0;
    aiUVTransform uvTransform;
    aiReturn result = material->Get(AI_MATKEY_UVWSRC(aiTextureType_BASE_COLOR, 0), colorChannel);
    material->Get(AI_MATKEY_UVTRANSFORM(aiTextureType_BASE_COLOR, 0), uvTransform);
    if (result != AI_SUCCESS)
    {
        material->Get(AI_MATKEY_UVWSRC(aiTextureType_DIFFUSE, 0), colorChannel);
        material->Get(AI_MATKEY_UVTRANSFORM(aiTextureType_DIFFUSE, 0), uvTransform);
    }
    material->Get(AI_MATKEY_UVWSRC(aiTextureType_EMISSIVE, 0), emissionChannel);
    //int colorChannel = 0;
    //int emissionChannel = 1;

    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        if (mesh->HasTextureCoords(colorChannel))
        {
            vertex.colorUV.x = mesh->mTextureCoords[colorChannel][i].x * uvTransform.mScaling.x + uvTransform.mTranslation.x;
            vertex.colorUV.y = mesh->mTextureCoords[colorChannel][i].y * uvTransform.mScaling.y + uvTransform.mTranslation.y;
        }
        else
        {
            vertex.colorUV.x = 0.0f;
            vertex.colorUV.y = 0.0f;
        }

        if (mesh->HasTextureCoords(emissionChannel))
        {
            vertex.emissiveUV.x = mesh->mTextureCoords[emissionChannel][i].x;
            vertex.emissiveUV.y = mesh->mTextureCoords[emissionChannel][i].y;
        }
        else
        {
            vertex.emissiveUV.x = 0.0f;
            vertex.emissiveUV.y = 0.0f;
        }

        if (mesh->mNormals)
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        else
        {
            vertex.normal.x = 0.0f;
            vertex.normal.y = 0.0f;
            vertex.normal.z = 0.0f;
        }
        m_Vertices.push_back(vertex);
    }
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++) m_Indices.push_back(face.mIndices[j]);
    }

    m_VertexArray = std::make_shared<Graphics::VertexArray>();
    std::shared_ptr<Graphics::VertexBuffer> vbo = std::make_shared<Graphics::VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
    vbo->SetLayout({
        {"a_Position", Graphics::VertexDataType::Float3 },
        {"a_TexCoord", Graphics::VertexDataType::Float2 },
        {"a_EmissiveCoord", Graphics::VertexDataType::Float2 },
        {"a_Normal", Graphics::VertexDataType::Float3 }
    });
    m_VertexArray->SetVertexBuffer(vbo);
    std::shared_ptr<Graphics::IndexBuffer> ibo = std::make_shared<Graphics::IndexBuffer>(m_Indices.data(), m_Indices.size());
    m_VertexArray->SetIndexBuffer(ibo);
}

void Mesh::SortTransparentMesh(const glm::mat4 view)
{
    glm::mat4 cameraTransform = glm::inverse(view);
    glm::vec3 cameraPos = glm::vec3(cameraTransform[3]);
    uint32_t triangleCount = m_Indices.size() / 3;
    std::vector<TriangleSortKey> triangles(triangleCount);

    for (uint32_t i = 0; i < triangleCount; i++)
    {
        uint32_t index0 = m_Indices[i * 3];
        uint32_t index1 = m_Indices[i * 3 + 1];
        uint32_t index2 = m_Indices[i * 3 + 2];

        glm::vec3 centroid = (m_Vertices[index0].position + m_Vertices[index1].position + m_Vertices[index2].position) / 3.0f;

        triangles[i].i0 = index0;
        triangles[i].i1 = index1;
        triangles[i].i2 = index2;
        triangles[i].distanceSq = glm::distance2(centroid, cameraPos);
    }

    std::sort(triangles.begin(), triangles.end(), [](const TriangleSortKey& a, const TriangleSortKey& b)
    {
        return a.distanceSq < b.distanceSq;
    });

    for (uint32_t i = 0; i < triangleCount; i++)
    {
        m_Indices[i * 3] = triangles[i].i0;
        m_Indices[i * 3 + 1] = triangles[i].i1;
        m_Indices[i * 3 + 2] = triangles[i].i2;
    }
    m_VertexArray->GetIndexBuffer()->SetData(m_Indices.data(), m_Indices.size());
}
