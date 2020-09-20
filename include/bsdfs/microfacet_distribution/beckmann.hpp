//
// Created by goksu on 6/24/20.
//

#pragma once

#include <glm/vec3.hpp>

namespace soleil::bsdfs::mf_distributions
{
class beckmann
{
public:
    beckmann(float alpha_x, float alpha_y)
        : m_alpha_x(alpha_x)
        , m_alpha_y(alpha_y)
    {}
    float D(const glm::vec3& w_h) const;
    float D(float cos_theta) const;

    float G1(const glm::vec3& w) const;
    float G(const glm::vec3& w_o, const glm::vec3& w_i) const;

private:
    float m_alpha_x;
    float m_alpha_y;

    float lambda(const glm::vec3& w) const;
    float alpha(const glm::vec3& w) const;
};
} // namespace soleil::bsdfs::mf_distributions
