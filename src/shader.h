#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <istream>
#include <utility>

namespace graphics {

// Encapsulates a shader program
class Shader {
public:
    // Construct a shader from a vertex and fragment shader.
    Shader(const std::istream& vert, const std::istream& frag);

    // Deletion
    ~Shader() { release(); };
    void release();

    // No copying (how?)
    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;

    // Moving is fine
    Shader(Shader&& other) : m_id(other.m_id) { other.m_id = 0; }
    Shader& operator=(Shader&& other) { std::swap(m_id, other.m_id); return *this; }

    // Get the OpenGL ID of the shader.
    unsigned get() const { return m_id; }
    operator unsigned() const { return m_id; }

private:
    unsigned m_id;
};

}

#endif
