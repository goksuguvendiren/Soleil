//
// Created by goksu on 5/20/20.
//

#pragma once

#include "sampler2D.hpp"

namespace soleil::textures
{
class checkerboard : public sampler2D
{
    glm::vec3 m_on_color;
    glm::vec3 m_off_color;

    int m_res_u;
    int m_res_v;

public:
    checkerboard(const glm::vec3& on_color, const glm::vec3& off_color, int res_u, int res_v);

    [[nodiscard]] glm::vec3 get_sample(const glm::vec2& uv) const override;
    [[nodiscard]] glm::vec3 get_sample(float u, float v) const override;
};
}