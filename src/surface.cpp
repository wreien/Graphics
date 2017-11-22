#include "surface.h"
#include <glm/glm.hpp>
#include <numeric>

namespace graphics::surface {

namespace {
    float bspline_coefficient(int m, int k, float t, const float* knot) {
        if (!m) return (knot[k] < t && t <= knot[k+1]) ? 1.f : 0.f;

        float x1 = (t - knot[k]) / (knot[m+k] - knot[k]);
        float n1 = bspline_coefficient(m-1, k, t, knot);

        float x2 = (knot[m+k+1] - t) / (knot[m+k+1] - knot[k+1]);
        float n2 = bspline_coefficient(m-1, k+1, t, knot);

        if (!std::isfinite(x1)) x1 = 0;
        if (!std::isfinite(x2)) x2 = 0;

        return x1 * n1 + x2 * n2;
    }

    float bspline_coefficient_derived(int m, int k, float t, const float* knot) {
        double x1 = m / (knot[m+k] - knot[k]);
        double n1 = bspline_coefficient(m-1, k, t, knot);

        double x2 = m / (knot[m+k+1] - knot[k+1]);
        double n2 = bspline_coefficient(m-1, k+1, t, knot);

        if (!std::isfinite(x1)) x1 = 0;
        if (!std::isfinite(x2)) x2 = 0;

        return x1 * n1 - x2 * n2;
    }
}

Values bspline(float s, float t, const Controls& c) {
    if (s <= 0.f) s = 1e-10f;
    if (t <= 0.f) t = 1e-10f;

    Values val;
    constexpr unsigned m = 3;

    std::vector<float> knotW;
    std::vector<float> knotH;
    knotW.reserve(m + c.width + 1);
    knotH.reserve(m + c.depth + 1);

    for (unsigned k = 0; k < m; k++)
        knotW.push_back(0);
    for (unsigned k = m; k <= c.width; k++)
        knotW.push_back(static_cast<float>(k - m) / (c.width - m));
    for (unsigned k = 0; k < m; k++)
        knotW.push_back(1);

    for (unsigned k = 0; k < m; k++)
        knotH.push_back(0);
    for (unsigned k = m; k <= c.depth; k++)
        knotH.push_back(static_cast<float>(k - m) / (c.depth - m));
    for (unsigned k = 0; k < m; k++)
        knotH.push_back(1);

    for (unsigned i = 0; i < c.width; i++) {
        for (unsigned j = 0; j < c.depth; j++) {
            glm::vec3 control { i, c.points[i * c.depth + j], j };

            float P = bspline_coefficient(m, i, s, knotW.data()) *
                      bspline_coefficient(m, j, t, knotH.data());

            float S = bspline_coefficient_derived(m, i, s, knotW.data()) *
                      bspline_coefficient(m, j, t, knotH.data());

            float T = bspline_coefficient(m, i, s, knotW.data()) *
                      bspline_coefficient_derived(m, j, t, knotH.data());

            val.position  += P * control;
            val.x_tangent += S * control;
            val.z_tangent += T * control;
        }
    }

    val.x_tangent = glm::normalize(val.x_tangent);
    val.z_tangent = glm::normalize(val.z_tangent);

    return val;
}

}
