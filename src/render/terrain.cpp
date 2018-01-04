#include "terrain.h"
#include <cmath>
#include <tuple>
#include <glm/glm.hpp>

#ifdef DEBUG
#include <limits>
#endif

namespace render {

Terrain::Terrain(unsigned width, unsigned depth, std::vector<float> heightmap)
    : m_width { width }
    , m_depth { depth }
    , m_heightmap { std::move(heightmap) }
    , m_tex { "terrain.png" }
    , m_mesh { std::nullopt }
{
#ifdef DEBUG
    if (m_width * m_depth > std::numeric_limits<unsigned short>::max())
        throw std::out_of_range("Too many vertices");
#endif

    unsigned tilesWide = m_width - 1;
    unsigned tilesDeep = m_depth - 1;

    unsigned slicesWide = tilesWide * slices_per_tile;
    unsigned slicesDeep = tilesDeep * slices_per_tile;

    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;
    vertices.reserve((slicesWide + 1) * (slicesDeep + 1));
    indices.reserve(slicesWide * slicesDeep * 2 * 3);

    float x_inc = 1.0f / slicesWide;
    float z_inc = 1.0f / slicesDeep;

    // Calculate vertex positions, including the ends
    for (unsigned col = 0; col <= slicesWide; ++col) {
        for (unsigned row = 0; row <= slicesDeep; ++row) {
            auto [pos, tx, tz] = bspline(col * x_inc, row * z_inc);

            auto norm = glm::cross(tx, tz);
            auto tex = glm::vec2{ pos.x, pos.z };

            vertices.push_back({ pos, norm, tex });
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

    m_mesh.emplace(std::move(vertices), std::move(indices));
}

float Terrain::altitude(float x, float z) const {
    auto st = retrieveST(x, z);
    auto [pos, tx, tz] = bspline(st.s, st.t);

    return pos.y;
}

glm::vec2 Terrain::retrieveST(float x, float z) const {
    auto& data = m_mesh->getVertices();

    auto different = [](float a, float b) { return std::fabsf(a - b) > 1e-5; };

    auto comp_x = [different](const Vertex& a, const Vertex& b) {
        if (different(a.position.x, b.position.x))
            return a.position.x < b.position.x;
        return different(a.position.z, b.position.z) && a.position.z < b.position.z;
    };

    auto comp_z = [different](const Vertex& a, const Vertex& b) {
        return different(a.position.z, b.position.z) && a.position.z < b.position.z;
    };

#if DEBUG
    if (!std::is_sorted(begin(data), end(data), comp_x))
        throw std::runtime_error("Bad sort comp!");
#endif

    const auto datam_depth = (m_depth - 1) * slices_per_tile + 1;

    // bounds checking
    const auto firstIter = begin(data) + datam_depth + 1;
    const auto lastIter  = end(data)   - datam_depth - 1;

    // get iter to point directly after given in both directions
    Vertex value; value.position = { x, 0, z };
    auto xIter   = std::lower_bound(firstIter, lastIter         , value, comp_x);
    auto closest = std::lower_bound(xIter    , xIter + datam_depth, value, comp_z);

    // recalculate the st used to generate this point
    auto iterPos = std::distance(begin(data), closest);
    glm::vec2 location = { iterPos / datam_depth, iterPos % datam_depth };

    glm::vec2 inc = {
        1.0f / ((m_width - 1) * slices_per_tile),
        1.0f / ((m_depth - 1) * slices_per_tile),
    };

    glm::vec2 st0 = {
        (location.s - 1) * inc.s,
        (location.t - 1) * inc.t,
    };

    const glm::vec3& current = closest->position;
    const glm::vec3& previous =
        data[(location.s - 1) * datam_depth + location.t - 1].position;

    // lerp from previous to current point's st
    return glm::clamp(glm::vec2 {
        st0.s + (x - previous.x) * inc.s / (current.x - previous.x),
        st0.t + (z - previous.z) * inc.t / (current.z - previous.z),
    }, 0.f, 1.f);
}

namespace {
    float bspline_coefficient(int m, int k, float t, const float* knot);
    float bspline_coefficient_derived(int m, int k, float t, const float* knot);
}

std::tuple<glm::vec3, glm::vec3, glm::vec3> Terrain::bspline(float s, float t) const {
    if (s <= 0.f) s = 1e-10f;
    if (t <= 0.f) t = 1e-10f;

    glm::vec3 position  { 0 };
    glm::vec3 x_tangent { 0 };
    glm::vec3 z_tangent { 0 };

    constexpr unsigned m = 3;

    std::vector<float> knotW;
    std::vector<float> knotH;
    knotW.reserve(m + m_width + 1);
    knotH.reserve(m + m_depth + 1);

    for (unsigned k = 0; k < m; k++)
        knotW.push_back(0);
    for (unsigned k = m; k <= m_width; k++)
        knotW.push_back(static_cast<float>(k - m) / (m_width - m));
    for (unsigned k = 0; k < m; k++)
        knotW.push_back(1);

    for (unsigned k = 0; k < m; k++)
        knotH.push_back(0);
    for (unsigned k = m; k <= m_depth; k++)
        knotH.push_back(static_cast<float>(k - m) / (m_depth - m));
    for (unsigned k = 0; k < m; k++)
        knotH.push_back(1);

    for (unsigned i = 0; i < m_width; i++) {
        for (unsigned j = 0; j < m_depth; j++) {
            glm::vec3 control { i, m_heightmap[i * m_depth + j], j };

            float P = bspline_coefficient(m, i, s, knotW.data()) *
                      bspline_coefficient(m, j, t, knotH.data());

            float S = bspline_coefficient_derived(m, i, s, knotW.data()) *
                      bspline_coefficient(m, j, t, knotH.data());

            float T = bspline_coefficient(m, i, s, knotW.data()) *
                      bspline_coefficient_derived(m, j, t, knotH.data());

            position  += P * control;
            x_tangent += S * control;
            z_tangent += T * control;
        }
    }

    x_tangent = glm::normalize(x_tangent);
    z_tangent = glm::normalize(z_tangent);

    return std::make_tuple(position, x_tangent, z_tangent);
}

namespace {

    float bspline_coefficient(int m, int k, float t, const float* knot) {
        if (!m) return (knot[k] < t && t <= knot[k+1]) ? 1.f : 0.f;

        float x1 = (t - knot[k]) / (knot[m+k] - knot[k]);
        if (std::isfinite(x1))
            x1 *= bspline_coefficient(m-1, k, t, knot);
        else
            x1 = 0;

        float x2 = (knot[m+k+1] - t) / (knot[m+k+1] - knot[k+1]);
        if (std::isfinite(x2))
            x2 *= bspline_coefficient(m-1, k+1, t, knot);
        else
            x2 = 0;

        return x1 + x2;
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

} // namespace

} // namespace render
