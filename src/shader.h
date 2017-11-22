#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <istream>
#include <utility>
#include <glm/fwd.hpp>

namespace graphics {

// Encapsulates a shader program
class Shader {
public:
    // Construct a shader from a vertex and fragment shader.
    Shader(const std::istream& vert, const std::istream& frag);
    Shader() : m_id{0} {}

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

    // Perform shader operations
    void use() const;
    void setUniform(const std::string& name, bool value) const;
    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, const glm::vec2& value) const;
    void setUniform(const std::string& name, const glm::vec3& value) const;
    void setUniform(const std::string& name, const glm::vec4& value) const;
    void setUniform(const std::string& name, const glm::mat2& value) const;
    void setUniform(const std::string& name, const glm::mat3& value) const;
    void setUniform(const std::string& name, const glm::mat4& value) const;

private:
    unsigned m_id;
};

}

#endif
