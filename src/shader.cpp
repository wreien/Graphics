#include "shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>

static unsigned compile(std::string_view shader, int type) {
    // Create the shader
    unsigned id = glCreateShader(type);

    // Compile it
    const char* data = shader.data();
    glShaderSource(id, 1, &data, nullptr);
    glCompileShader(id);

    // Check we succeeded
    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(id, 512, nullptr, log);
        std::cerr << "Error: failed to compile shader\n" << log << std::endl;
        std::exit(1);
    }

    return id;
}

namespace graphics {

Shader::Shader(const std::istream& vert, const std::istream& frag) {
    std::string vert_str { static_cast<const std::stringstream&>(
            std::stringstream() << vert.rdbuf()).str() };
    std::string frag_str { static_cast<const std::stringstream&>(
            std::stringstream() << frag.rdbuf()).str() };

    // Create the vertex and fragment shaders
    unsigned vert_id = compile(vert_str, GL_VERTEX_SHADER);
    unsigned frag_id = compile(frag_str, GL_FRAGMENT_SHADER);

    // Link the program
    m_id = glCreateProgram();
    glAttachShader(m_id, vert_id);
    glAttachShader(m_id, frag_id);
    glLinkProgram(m_id);

    // Check we succeeded
    int success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(m_id, 512, nullptr, log);
        std::cerr << "Error: failed to link shader program\n" << log << std::endl;
        std::exit(1);
    }

    // Validate program
    glValidateProgram(m_id);
    glGetProgramiv(m_id, GL_VALIDATE_STATUS, &success);
    if (!success) {
        std::cerr << "Error: failed to validate shader program\n";
        std::exit(1);
    }

    // Clean up
    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
}

void Shader::release() {
    glDeleteProgram(m_id);
    m_id = 0;
}

void Shader::use() const {
    glUseProgram(m_id);
}

void Shader::setUniform(const std::string& name, bool value) const {
    setUniform(name, (int)value);
}

void Shader::setUniform(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setUniform(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::setUniform(const std::string& name, const glm::mat2& value) const {
    glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()),
            1, GL_FALSE, &value[0][0]);
}

void Shader::setUniform(const std::string& name, const glm::mat3& value) const {
    glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()),
            1, GL_FALSE, &value[0][0]);
}

void Shader::setUniform(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()),
            1, GL_FALSE, &value[0][0]);
}

}
