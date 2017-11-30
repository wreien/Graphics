#include "level.h"
#include <json/json.h>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <algorithm>
#include <iterator>

#include <iostream>
#include <cstdlib>
#include <cstddef>

#include "surface.h"

namespace world {

Level::Level(std::string filename)
    : shader { std::ifstream("shaders/main.vert"),
               std::ifstream("shaders/main.frag") }
{
    load_from_file(filename);
}

void Level::load_from_file(std::string filename) {
    Json::Value root;
    (std::ifstream { filename }) >> root;

    width = root.get("width", 0).asUInt();
    depth = root.get("depth", 0).asUInt();

    if (width < 5 || depth < 5) {
        std::cerr << "Invalid size for " << filename << ": "
                  << width << ", " << depth << std::endl;
        std::exit(1);
    }

    Json::Value alts = root["altitude"];
    heightmap.clear();
    heightmap.reserve(width * depth);
    std::transform(std::begin(alts), std::end(alts), std::back_inserter(heightmap),
        [](auto node) { return node.asFloat(); });

    if (heightmap.size() != width * depth) {
        std::cerr << "Invalid altitude data given for " << filename << std::endl;
        std::exit(1);
    }

    generate_mesh();

    camera.setClamps({ width - 1, depth - 1 });
    this->move(graphics::Camera::Direction::Forward, 0);
}

void Level::generate_mesh() {
    unsigned tilesWide = width - 1;
    unsigned tilesDeep = depth - 1;

    unsigned slicesWide = tilesWide * slices_per_tile;
    unsigned slicesDeep = tilesDeep * slices_per_tile;

    data.clear();
    indices.clear();

    data.reserve((slicesWide + 1) * (slicesDeep + 1));
    indices.reserve(slicesWide * slicesDeep * 2 * 3);

    float x_inc = 1.0f / slicesWide;
    float z_inc = 1.0f / slicesDeep;

    // Calculate vertex positions, including the ends
    for (unsigned col = 0; col <= slicesWide; ++col) {
        for (unsigned row = 0; row <= slicesDeep; ++row) {
            auto [pos, tx, tz] = graphics::surface::bspline(
                    col * x_inc, row * z_inc, { heightmap, width, depth });
            auto norm = glm::cross(tx, tz);
            auto tex = glm::vec2{ pos.x / slicesWide, pos.z / slicesDeep };

            data.push_back({ pos, norm, tex });
        }
    }

    // Calculate indices, excluding the end
    for (unsigned col = 0; col < slicesWide; ++col) {
        for (unsigned row = 0; row < slicesDeep; ++row) {
            unsigned short a = (col + 0) * (slicesDeep + 1) + (row + 0);
            unsigned short b = (col + 0) * (slicesDeep + 1) + (row + 1);
            unsigned short c = (col + 1) * (slicesDeep + 1) + (row + 0);
            unsigned short d = (col + 1) * (slicesDeep + 1) + (row + 1);

            // top triangle
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);

            // bottom triangle
            indices.push_back(d);
            indices.push_back(c);
            indices.push_back(b);
        }
    }

    generate_VBOs();
}

void Level::generate_VBOs() {
    glGenVertexArrays(1, vaos);
    glGenBuffers(2, vbos);
    unsigned vsize = data.size() * sizeof(vertexData);
    unsigned isize = indices.size() * sizeof(unsigned short);

    glBindVertexArray(vaos[0]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, vsize, data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize, indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertexData),
                (void*) offsetof(vertexData, position));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertexData),
                (void*) offsetof(vertexData, normal));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertexData),
                (void*) offsetof(vertexData, texcoord));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}

void Level::render(const glm::mat4& projection) const {
    glm::mat4 modelview = projection * camera.getView();

    shader.use();
    shader.setUniform("modelview", modelview);

    glBindVertexArray(vaos[0]);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
}

glm::vec2 Level::retrieveST(float x, float z) const {
    auto different = [](float a, float b) { return std::fabsf(a - b) > 1e-5; };

    auto comp_x = [different](const vertexData& a, const vertexData& b) {
        if (different(a.position.x, b.position.x))
            return a.position.x < b.position.x;
        return different(a.position.z, b.position.z) && a.position.z < b.position.z;
    };

    auto comp_z = [different](const vertexData& a, const vertexData& b) {
        return different(a.position.z, b.position.z) && a.position.z < b.position.z;
    };

#if DEBUG
    if (!std::is_sorted(begin(data), end(data), comp_x))
        throw std::runtime_error("Bad sort comp!");
#endif

    const auto dataDepth = (depth - 1) * slices_per_tile + 1;

    // bounds checking
    const auto firstIter = begin(data) + dataDepth + 1;
    const auto lastIter  = end(data)   - dataDepth - 1;

    // get iter to point directly after given in both directions
    vertexData value; value.position = { x, 0, z };
    auto xIter   = std::lower_bound(firstIter, lastIter         , value, comp_x);
    auto closest = std::lower_bound(xIter    , xIter + dataDepth, value, comp_z);

    // recalculate the st used to generate this point
    auto iterPos = std::distance(begin(data), closest);
    glm::vec2 location = { iterPos / dataDepth, iterPos % dataDepth };

    glm::vec2 inc = {
        1.0f / ((width - 1) * slices_per_tile),
        1.0f / ((depth - 1) * slices_per_tile),
    };

    glm::vec2 st0 = {
        (location.s - 1) * inc.s,
        (location.t - 1) * inc.t,
    };

    const glm::vec3& current = closest->position;
    const glm::vec3& previous =
        data[(location.s - 1) * dataDepth + location.t - 1].position;

    // lerp from previous to current point's st
    return glm::clamp(glm::vec2 {
        st0.s + (x - previous.x) * inc.s / (current.x - previous.x),
        st0.t + (z - previous.z) * inc.t / (current.z - previous.z),
    }, 0.f, 1.f);
}

float Level::altitude(float x, float z) const {
    using namespace graphics::surface;

    auto st = retrieveST(x, z);
    const Controls c { heightmap, width, depth };
    return bspline(st.s, st.t, c).position.y;
}

}
