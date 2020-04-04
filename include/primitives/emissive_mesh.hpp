//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include "ray.hpp"
#include "utils.hpp"

#include <functional>
#include <glm/vec3.hpp>
#include <materials/base.hpp>
#include <optional>
#include <string>

namespace soleil
{
struct payload;
namespace primitives
{
class emissive_mesh
{
public:
    emissive_mesh(soleil::primitives::mesh geo, int idx)
        : geometry(std::move(geo))
        , material_idx(idx)
        , color({1.f, 1.f, 1.f})
    {}

    int material_idx;
    std::optional<soleil::payload> hit(const soleil::ray& ray) const;

    glm::vec3 direction(const glm::vec3& hit_pos) const
    {
        return glm::normalize(geometry.faces[0].vertices[0].poss - hit_pos);
    }

    glm::vec3 position()
    {
        auto u = get_random_float();
        auto v = get_random_float();

        auto corner = geometry.faces[0].vertices[0].poss;

        auto right_dir = geometry.faces[0].vertices[1].poss - corner;
        auto below_dir = geometry.faces[0].vertices[2].poss - corner;

        return corner + u * right_dir + v * below_dir;
    }

    float distance(const glm::vec3& hit_pos) const
    {
      return glm::length(geometry.faces[0].vertices[0].poss - hit_pos);
    }

    float attenuate(const glm::vec3& light_pos, const glm::vec3& hit_pos) const
    {
        auto distance = glm::length(light_pos - hit_pos);
        float c_1 = 0.25;
        float c_2 = 0.1;
        float c_3 = 0.01;

        auto attenuation = 1.f / float(c_1 + c_2 * distance + c_3 * distance * distance);
        return std::min(1.f, attenuation);
    }
    soleil::primitives::mesh geometry;
    glm::vec3 color;
};
} // namespace primitives
} // namespace soleil
