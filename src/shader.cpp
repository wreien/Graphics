#include "shader.h"
#include <glad/glad.h>

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

}
