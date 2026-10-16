#pragma once
#include <cstdint>
#include <filesystem>
#include <string>

#include "glm/fwd.hpp"
#include "glm/vec3.hpp"

namespace Graphics
{
    class Shader
    {
    public:
        Shader(const std::string& vertexShader, const std::string& fragmentShader);
        Shader(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader);
        ~Shader();

        void SetInt(const std::string& name, int value);
        void SetFloat(const std::string& name, float value);
        void SetFloat3Uniform(const std::string& name, const glm::vec3& value);
        void SetFloat4Uniform(const std::string& name, const glm::vec4& value);
        void SetMat4Uniform(const std::string& name, const glm::mat4& value);

        void Bind();
        void Unbind();

    private:
        uint32_t m_ShaderProgram;
    };
}
