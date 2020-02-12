//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include "ray.hpp"

#include <functional>
#include <glm/vec3.hpp>
#include <materials/base.hpp>
#include <optional>
#include <string>

namespace rtr
{
struct payload;
namespace primitives
{
class emissive_mesh
{
public:
    emissive_mesh(rtr::primitives::mesh geo, int idx)
        : geometry(std::move(geo))
        , material_idx(idx)
        , color({1.f, 1.f, 1.f})
    {}

    int material_idx;
    std::optional<rtr::payload> hit(const rtr::ray& ray) const;

    glm::vec3 direction(const glm::vec3& hit_pos) const
    {
      return glm::normalize(geometry.faces[0].vertices[0].poss - hit_pos);
    }

    float distance(const glm::vec3& hit_pos) const
    {
      return glm::length(geometry.faces[0].vertices[0].poss - hit_pos);
    }

    float attenuate(const glm::vec3& hit_pos) const
    {
        auto distance = glm::length(geometry.faces[0].vertices[0].poss - hit_pos);
        float c_1 = 0.25;
        float c_2 = 0.1;
        float c_3 = 0.01;

        auto attenuation = 1.f / float(c_1 + c_2 * distance + c_3 * distance * distance);
        return std::min(1.f, attenuation);
    }
    rtr::primitives::mesh geometry;
    glm::vec3 color;
};
} // namespace primitives
} // namespace rtr
