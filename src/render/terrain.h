#ifndef GRAPHICS_TERRAIN_H_INCLUDED
#define GRAPHICS_TERRAIN_H_INCLUDED

#include <tuple>
#include <vector>
#include <optional>
#include <glm/vec2.hpp>

#include "mesh.h"
#include "texture.h"

namespace render {

class Terrain {
    static constexpr unsigned slices_per_tile = 16;

public:
    Terrain(unsigned width, unsigned depth, std::vector<float> heightmap);

    void render() const { m_tex.use(); m_mesh->render(); }
    float altitude(float x, float z) const;
    auto size() const { return std::make_pair(m_width, m_depth); }

private:
    glm::vec2 retrieveST(float x, float z) const;
    auto bspline(float s, float t) const
        -> std::tuple<glm::vec3, glm::vec3, glm::vec3>;

    unsigned m_width;
    unsigned m_depth;
    std::vector<float> m_heightmap;

    Texture m_tex;
    std::optional<Mesh> m_mesh; // delayed construction: should always exist
};

}

#endif
