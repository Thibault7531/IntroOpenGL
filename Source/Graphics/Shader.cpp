#include "Shader.h"

#include <fstream>
#include <vector>

#include "glad/gl.h"
#include "glm/gtc/type_ptr.hpp"

namespace Graphics
{
    Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader)
    {
        GLint vshader = glCreateShader(GL_VERTEX_SHADER);
        const GLchar* vShaderStr = vertexShader.c_str();
        glShaderSource(vshader, 1, &vShaderStr, nullptr);
        glCompileShader(vshader);

        GLint success = 0;
        glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(vshader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(vshader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(vshader);

            std::printf("%s", infoLog.data());
            return;
        }

        GLint fshader = glCreateShader(GL_FRAGMENT_SHADER);
        const GLchar* fShaderStr = fragmentShader.c_str();
        glShaderSource(fshader, 1, &fShaderStr, nullptr);
        glCompileShader(fshader);

        success = 0;
        glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(fshader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(fshader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(fshader);

            std::printf("%s", infoLog.data());
            return;
        }

        m_ShaderProgram = glCreateProgram();
        glAttachShader(m_ShaderProgram, vshader);
        glAttachShader(m_ShaderProgram, fshader);
        glLinkProgram(m_ShaderProgram);

        success = 0;
        glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(m_ShaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(m_ShaderProgram, maxLength, &maxLength, &infoLog[0]);

            glDeleteProgram(m_ShaderProgram);
            glDeleteShader(vshader);
            glDeleteShader(fshader);

            std::printf("%s", infoLog.data());
            return;
        }
    }

    Shader::Shader(const std::filesystem::path& vertexShader, const std::filesystem::path& fragmentShader)
    {
        GLint vshader = glCreateShader(GL_VERTEX_SHADER);
        std::ifstream vShaderFile = std::ifstream(vertexShader);
        std::ostringstream oss;
        oss << vShaderFile.rdbuf();
        std::string vShaderSource = oss.str();
        const GLchar* vShaderStr = vShaderSource.c_str();
        glShaderSource(vshader, 1, &vShaderStr, nullptr);
        glCompileShader(vshader);
        vShaderFile.close();

        GLint success = 0;
        glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(vshader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(vshader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(vshader);

            std::printf("%s", infoLog.data());
            return;
        }

        GLint fshader = glCreateShader(GL_FRAGMENT_SHADER);
        std::ifstream fShaderFile = std::ifstream(fragmentShader);
        oss = {};
        oss << fShaderFile.rdbuf();
        std::string fShaderSource = oss.str();
        const GLchar* fShaderStr = fShaderSource.c_str();
        glShaderSource(fshader, 1, &fShaderStr, nullptr);
        glCompileShader(fshader);
        fShaderFile.close();

        success = 0;
        glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(fshader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(fshader, maxLength, &maxLength, &infoLog[0]);

            glDeleteShader(fshader);

            std::printf("%s", infoLog.data());
            return;
        }

        m_ShaderProgram = glCreateProgram();
        glAttachShader(m_ShaderProgram, vshader);
        glAttachShader(m_ShaderProgram, fshader);
        glLinkProgram(m_ShaderProgram);

        success = 0;
        glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(m_ShaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(m_ShaderProgram, maxLength, &maxLength, &infoLog[0]);

            glDeleteProgram(m_ShaderProgram);
            glDeleteShader(vshader);
            glDeleteShader(fshader);

            std::printf("%s", infoLog.data());
            return;
        }
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_ShaderProgram);
    }

    void Shader::SetInt(const std::string& name, int value)
    {
        uint32_t location = glGetUniformLocation(m_ShaderProgram, name.c_str());
        glUniform1i(location, value);
    }

    void Shader::SetFloat3Uniform(const std::string& name, const glm::vec3& value)
    {
        uint32_t location = glGetUniformLocation(m_ShaderProgram, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void Shader::SetFloat4Uniform(const std::string& name, const glm::vec4& value)
    {
        uint32_t location = glGetUniformLocation(m_ShaderProgram, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void Shader::SetMat4Uniform(const std::string& name, const glm::mat4& value)
    {
        uint32_t location = glGetUniformLocation(m_ShaderProgram, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::Bind()
    {
        glUseProgram(m_ShaderProgram);
    }

    void Shader::Unbind()
    {
        glUseProgram(0);
    }
}
