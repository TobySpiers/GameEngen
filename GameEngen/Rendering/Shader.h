#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    Shader(const Shader&)            = delete;
    Shader& operator=(const Shader&) = delete;

    void Use() const;

    void SetInt  (const std::string& name, int               value) const;
    void SetFloat(const std::string& name, float              value) const;
    void SetVec2 (const std::string& name, const glm::vec2&   value) const;
    void SetVec3 (const std::string& name, const glm::vec3&   value) const;
    void SetVec4 (const std::string& name, const glm::vec4&   value) const;
    void SetMat4 (const std::string& name, const glm::mat4&   value) const;

private:
    GLuint programId = 0;

    GLint GetUniformLocation(const std::string& name) const;

    static GLuint      CompileShader(GLenum type, const std::string& path);
    static std::string ReadFile     (const std::string& path);
};
