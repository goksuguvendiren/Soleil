//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include <functional>
#include <string>
#include <optional>
#include <glm/vec3.hpp>
#include <materials/base.hpp>
#include "ray.hpp"

namespace rtr
{
struct payload;
namespace primitives
{
class emissive_mesh
{
public:
    emissive_mesh(rtr::primitives::mesh geo, int idx)
        : geometry(std::move(geo)), material_idx(std::move(idx))
    {
    }

    int material_idx;
    std::optional<rtr::payload> hit(const rtr::ray &ray) const;

    rtr::primitives::mesh geometry;
};
} // namespace primitives
} // namespace rtr
