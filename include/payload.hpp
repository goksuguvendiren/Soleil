//
// Created by Göksu Güvendiren on 2019-05-10.
//
#pragma once

#include "ray.hpp"

#include <glm/vec3.hpp>
#include <primitives/mesh.hpp>
#include <primitives/sphere.hpp>
#include <variant>

namespace soleil
{
    struct payload
    {
        glm::vec3 hit_normal;
        glm::vec3 hit_pos;
        soleil::ray ray;
        float param = std::numeric_limits<float>::infinity();
        int material_idx;
        glm::vec2 texture_coords;

        int obj_id;
        std::optional<glm::vec3> emission = std::nullopt;
    };
} // namespace soleil
