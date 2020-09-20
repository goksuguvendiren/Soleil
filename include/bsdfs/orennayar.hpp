//
// Created by goksu on 6/18/20.
//

#pragma once

#include "bsdf.hpp"
#include "materials/base.hpp"

#include <glm/vec3.hpp>
#include <payload.hpp>
#include <scene.hpp>
#include <string>

namespace soleil::bsdfs
{
class orennayar
{
public:
    orennayar(const glm::vec3& albedo, soleil::radians sigma, std::string name);

    glm::vec3 f(const scene& scene, const payload& pld, const glm::vec3& light_direction) const;

    glm::vec3 sample(const glm::vec3& hit_normal, const payload& pld) const;

private:
    glm::vec3 m_albedo;
    soleil::radians m_sigma;
    std::string m_name;

    float m_sigma_squared;
    float m_A;
    float m_B;
};
} // namespace soleil::materials