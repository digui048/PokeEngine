#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm/glm.hpp>

namespace Poke
{
    struct ShaderProgramSource
    {
        std::string vertexSource;
        std::string fragmentSource;
    };

    class Shader
    {
    public:
        Shader(const std::string& path);
        ~Shader();

        void Bind() const;
        void Unbind() const;

        void SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3) const;
        void SetUniformMatrix4f(const std::string &name, const glm::mat4 &mat) const;
        void SetUniform1f(const std::string &name, float value) const;
        void SetUniform1i(const std::string &name, unsigned int value) const;
        void SetUniform3f(const std::string &name, float v0, float v1, float v2) const;

    private:
        unsigned int CompileShader(unsigned int type, const std::string &source);
        unsigned int CreateShader(const std::string &vertexShader, const std::string &fragmentShader);
        int GetUniformLocation(const std::string &name) const;

        ShaderProgramSource ParseShader(const std::string &filepath);

    private:
        unsigned int m_rendererID;
        std::string m_filePath;
    };
}

#endif