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

namespace world {

Level::Level(std::string filename)
    : m_shader { "shaders/main.vert", "shaders/main.frag" }
    , m_camera { }
    , m_terrain { std::nullopt }
{
    load_from_file(filename);
}

void Level::load_from_file(std::string filename) {
    Json::Value root;
    (std::ifstream { filename }) >> root;

    // we handle in column-major order, but for readability we will
    // pretend we store in row-major order for the json files.
    unsigned width = root.get("depth", 0).asUInt();
    unsigned depth = root.get("width", 0).asUInt();

    if (width < 5 || depth < 5) {
        std::cerr << "Invalid size for " << filename << ": "
                  << width << ", " << depth << std::endl;
        std::exit(1);
    }

    Json::Value alts = root["altitude"];

    std::vector<float> heightmap;
    heightmap.clear();
    heightmap.reserve(width * depth);
    std::transform(std::begin(alts), std::end(alts), std::back_inserter(heightmap),
        [](auto node) { return node.asFloat(); });

    if (heightmap.size() != width * depth) {
        std::cerr << "Invalid altitude data given for " << filename << std::endl;
        std::exit(1);
    }

    m_terrain.emplace(width, depth, std::move(heightmap));

    m_camera.setClamps({ width - 1, depth - 1 });
    this->move(Direction::Forward, 0);
}

void Level::render(const glm::mat4& projection) const {
    glm::mat4 modelview = projection * m_camera.getView();

    m_shader.use();
    m_shader.setUniform("modelview", modelview);

    m_terrain->render();
}

void Level::move(Direction dir, float dt) {
    m_camera.move(dir, dt, [=](float x, float z) {
        return m_terrain->altitude(x, z);
    });
}

void Level::tilt(float dx, float dy) {
    m_camera.tilt(dx, dy);
}

}
