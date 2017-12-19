#ifndef GRAPHICS_MESH_H_INCLUDED
#define GRAPHICS_MESH_H_INCLUDED

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace render {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned short> indices);
    ~Mesh();

    Mesh(Mesh&& other);
    Mesh& operator=(Mesh&& other);

    void render() const;

    const std::vector<Vertex>& getVertices() const { return m_vertices; }

private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned short> m_indices;

    unsigned m_vao;
    unsigned m_vbo;
    unsigned m_ebo;
};

}

#endif // RENDER_MESH_H_INCLUDED
