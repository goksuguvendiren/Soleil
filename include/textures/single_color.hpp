//
// Created by goksu on 6/23/20.
//

#pragma once

#include "sampler2D.hpp"

namespace soleil::textures
{
class single_color : public sampler2D
{
    glm::vec3 m_color;

public:
    explicit single_color(const glm::vec3& color) : m_color(color) {}
    [[nodiscard]] glm::vec3 get_sample(const glm::vec2& uv) const override { return m_color; }
    [[nodiscard]] glm::vec3 get_sample(float u, float v) const override { return m_color; }
};
}