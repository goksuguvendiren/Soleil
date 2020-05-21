//
// Created by goksu on 5/21/20.
//

#include <cmath>
#include <textures/checkerboard.hpp>

namespace soleil::textures
{
checkerboard::checkerboard(const glm::vec3& on_color, const glm::vec3& off_color, int res_u, int res_v) :
    m_on_color(on_color), m_off_color(off_color), m_res_u(res_u), m_res_v(res_v)
{}

glm::vec3 checkerboard::get_sample(const glm::vec2& uv) const
{
    return get_sample(uv.x, uv.y);
}

glm::vec3 checkerboard::get_sample(float u, float v) const
{
    float w_u = 1.f / (float)m_res_u;
    float w_v = 1.f / (float)m_res_v;

    int u_index = std::floor(u / w_u);
    int v_index = std::floor(v / w_v);

    auto u_on = u_index % 2 == 1;
    auto v_on = v_index % 2 == 1;

    if(u_on ^ v_on) return m_off_color;
    return m_on_color;
}
}