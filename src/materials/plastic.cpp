//
// Created by goksu on 5/27/20.
//

#include "materials/plastic.hpp"

#include <payload.hpp>

namespace soleil::materials
{
glm::vec3 plastic::f(const scene &scene, const payload &pld, const glm::vec3& incoming_light_direction) const
{
    auto kd = m_kd;
    if (m_texture)
    {
        kd = m_texture->get_sample(pld.texture_coords);
    }

    // TODO: multiply by the ks
    return m_brdf.f(scene, pld, incoming_light_direction);
}

glm::vec3 plastic::sample(const glm::vec3 &hit_normal, const payload &pld) const
{
    return sample_hemisphere(hit_normal);
}
}