//
// Created by Göksu Güvendiren on 2019-05-10.
//
#pragma once

#include "ray.hpp"

#include <glm/vec3.hpp>
#include <primitives/mesh.hpp>
#include <primitives/sphere.hpp>
#include <variant>

namespace rtr
{
struct payload
{
    glm::vec3 hit_normal;
    glm::vec3 hit_pos;
    rtr::ray ray;
    float param = std::numeric_limits<float>::infinity();
    const rtr::material* material;
    glm::vec2 texture_coords;
    int obj_id;
};
} // namespace rtr
