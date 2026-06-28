#include "Shader.h"
#include "Poke/Core/Assert.h"

#include <fstream>
#include <sstream>
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace Poke;

Shader::Shader(const std::string &path) : m_filePath(path), m_rendererID(0)
{
    POKE_CORE_INFO("Loading shader file from path: {0}", path);
    ShaderProgramSource src = ParseShader(path);
    m_rendererID = CreateShader(src.vertexSource, src.fragmentSource);

    POKE_ASSERT(m_rendererID != 0, "Failed to create shader program for: {0}", path);
}

Shader::~Shader()
{
    glDeleteProgram(m_rendererID);
}

void Shader::Bind() const
{
    glUseProgram(m_rendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3) const
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformMatrix4f(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniform1f(const std::string &name, float value) const
{
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform1i(const std::string &name, unsigned int value) const
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform3f(const std::string &name, float v0, float v1, float v2) const
{
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
}

int Shader::GetUniformLocation(const std::string &name) const
{
    int location = glGetUniformLocation(m_rendererID, name.c_str());
    return location;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string &source)
{
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length);
        glGetShaderInfoLog(id, length, &length, message.data());

        std::string shaderTypeStr = (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
        POKE_CORE_ERROR("Failed to compile {0} shader!", shaderTypeStr);
        POKE_CORE_ERROR("GLSL Error Message: {0}", message.data());

        glDeleteShader(id);
        POKE_ASSERT(false, "Shader compilation failed!");
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
{
    POKE_ASSERT(!vertexShader.empty(), "Vertex shader source code is empty!");
    POKE_ASSERT(!fragmentShader.empty(), "Fragment shader source code is empty!");

    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

ShaderProgramSource Shader::ParseShader(const std::string &filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1,
        VERTEX,
        FRAGMENT
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return {ss[0].str(), ss[1].str()};
}