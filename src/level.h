#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <string>
#include <vector>
#include <utility>
#include <optional>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "render/shader.h"
#include "render/terrain.h"
#include "camera.h"

namespace world {

// Encapsulates a world loaded from file
class Level {
public:
    // Load the level from the JSON file given by `filename'.
    Level(std::string filename);
    void load_from_file(std::string filename);

    void render(const glm::mat4& projection) const;

    using Direction = Camera::Direction;
    void move(Direction dir, float dt);
    void tilt(float dx, float dy);

private:
    Camera m_camera;

    render::Shader m_shader;
    std::optional<render::Terrain> m_terrain; // delayed construction
};

}

#endif
