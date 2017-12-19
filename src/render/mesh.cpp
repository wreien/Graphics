#include "mesh.h"

#include <utility> // move
#include <cstddef> // offsetof

#include <glad/glad.h>

namespace render {

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned short> indices)
    : m_vertices { std::move(vertices) }
    , m_indices { std::move(indices) }
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    unsigned vsize = m_vertices.size() * sizeof(Vertex);
    unsigned isize = m_indices.size() * sizeof(unsigned short);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vsize, m_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize, m_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (void*) offsetof(Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (void*) offsetof(Vertex, normal));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            (void*) offsetof(Vertex, texcoord));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
}

Mesh::Mesh(Mesh&& other)
    : m_vertices { std::move(other.m_vertices) }
    , m_indices { std::move(other.m_indices) }
    , m_vao { other.m_vao }
    , m_vbo { other.m_vbo }
    , m_ebo { other.m_ebo }
{
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
}

Mesh& Mesh::operator=(Mesh&& other) {
    m_vertices.swap(other.m_vertices);
    m_indices.swap(other.m_indices);
    std::swap(m_vao, other.m_vao);
    std::swap(m_vbo, other.m_vbo);
    std::swap(m_ebo, other.m_ebo);

    return *this;
}

void Mesh::render() const {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, 0);
}

}
