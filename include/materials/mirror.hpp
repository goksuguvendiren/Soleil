//
// Created by goksu on 5/23/20.
//

#pragma once

#include "materials/base.hpp"

namespace soleil
{
namespace materials
{
class mirror : public base
{
public:
    mirror(const glm::vec3& albedo, const std::string& name) : m_albedo(albedo), m_name(name)
    {}

    bool is_mirror() const override { return true; }

    glm::vec3 f(const scene& scene, const payload& pld, const glm::vec3& light_direction) const override;

    glm::vec3 sample(const glm::vec3 &hit_normal, const payload &pld) const override;

private:
    glm::vec3 m_albedo;
    std::string m_name;
};
} // namespace materials
} // namespace soleil