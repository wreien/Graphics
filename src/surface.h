#ifndef SURFACE_H_INCLUDED
#define SURFACE_H_INCLUDED

#include <glm/vec3.hpp>
#include <vector>

namespace graphics::surface {

struct Values {
    glm::vec3 position;
    glm::vec3 x_tangent;
    glm::vec3 z_tangent;
};

struct Controls {
    const std::vector<float>& points;
    unsigned width;
    unsigned depth;
};

Values bspline(float s, float t, const Controls& c);

}

#endif
