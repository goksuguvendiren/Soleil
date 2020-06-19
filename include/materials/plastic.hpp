//
// Created by goksu on 5/27/20.
//

#pragma once

#include "base.hpp"
namespace soleil::materials
{
class plastic : public base
{
    plastic(const glm::vec3& albedo, const glm::vec3& specular, float alpha, const std::string& name) :
    m_albedo(albedo), m_specular(specular), m_alpha(alpha), m_name(name)
    {}

    glm::vec3 f(const scene& scene, const payload& pld, const glm::vec3& light_direction) const override;

    glm::vec3 sample(const glm::vec3 &hit_normal, const payload &pld) const override;

private:
    glm::vec3 m_albedo;
    glm::vec3 m_specular;
    float m_alpha;
    std::string m_name;

};
}