//
// Created by goksu on 5/27/20.
//

#include "materials/plastic.hpp"

#include <payload.hpp>

namespace soleil::materials
{
glm::vec3 reflect(const glm::vec3 &hit_normal, const glm::vec3 &incoming_light_direction)
{
//    return hit_normal;
    auto w_o = -incoming_light_direction;
    auto cos_theta = glm::dot(hit_normal, w_o);

    assert(cos_theta > 0);

    auto reflected_ray = - w_o + 2 * cos_theta * hit_normal;
    assert(glm::dot(reflected_ray, hit_normal) >= 0);
    return reflected_ray;
}

glm::vec3 plastic::f(const scene &scene, const payload &pld, const glm::vec3& incoming_light_direction) const
{
    glm::vec3 albedo = diffuse;
    if (m_texture)
        albedo = m_texture->get_sample(pld.texture_coords);

    auto lambertian = albedo / glm::pi<float>();

    auto light_reflected = reflect(pld.hit_normal, incoming_light_direction);
    auto cos_theta = glm::dot(pld.hit_normal, light_reflected);

    auto specular = m_specular * (m_alpha + 2) / (2 * glm::pi<float>()) * glm::pow(cos_theta, m_alpha);

    return lambertian + specular;
}

glm::vec3 plastic::sample(const glm::vec3 &hit_normal, const payload &pld) const
{}
}