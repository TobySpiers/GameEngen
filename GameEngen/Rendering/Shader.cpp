#include "Shader.h"

#include "Log.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>

Shader::Shader(const std::string& vertPath, const std::string& fragPath)
{
    GLuint vertShader = CompileShader(GL_VERTEX_SHADER,   vertPath);
    GLuint fragShader = CompileShader(GL_FRAGMENT_SHADER, fragPath);

    programId = glCreateProgram();
    glAttachShader(programId, vertShader);
    glAttachShader(programId, fragShader);
    glLinkProgram(programId);

    GLint success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(programId, 512, nullptr, infoLog);
        Log::Error(LogCategory::Graphics, "Shader link error:\n" + std::string(infoLog));
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

Shader::~Shader()
{
    glDeleteProgram(programId);
}

void Shader::Use() const
{
    glUseProgram(programId);
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

GLint Shader::GetUniformLocation(const std::string& name) const
{
    return glGetUniformLocation(programId, name.c_str());
}

GLuint Shader::CompileShader(GLenum type, const std::string& path)
{
    const std::string source     = ReadFile(path);
    const char*       sourceCStr = source.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char        infoLog[512];
        const char* typeName = (type == GL_VERTEX_SHADER) ? "vertex" : "fragment";
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        Log::Error(LogCategory::Graphics, "Shader compile error (" + std::string(typeName) + ": " + path + "):\n" + std::string(infoLog));
    }

    return shader;
}

std::string Shader::ReadFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        Log::Error(LogCategory::Graphics, "Failed to open shader file: " + path);
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
