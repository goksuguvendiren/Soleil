//
// Created by Göksu Güvendiren on 6.03.2020.
//

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>

namespace soleil::textures
{
class sampler2D
{
public:
    virtual ~sampler2D() = default;
    virtual glm::vec3 get_sample(const glm::vec2& uv) const = 0;
    virtual glm::vec3 get_sample(float u, float v) const = 0;
};
}