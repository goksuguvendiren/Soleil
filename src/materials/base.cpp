//
// Created by goksu on 6/22/20.
//

#include "materials/base.hpp"

#include "payload.hpp"
#include "scene.hpp"

glm::vec3 soleil::materials::base::f(const soleil::scene& scene, const soleil::payload& pld, const glm::vec3& light_direction) const
{
    std::cerr << "Deprecated, use the matte material instead!\n";
    glm::vec3 albedo = diffuse;
    if (m_texture)
        albedo = m_texture->get_sample(pld.texture_coords);

    return albedo / glm::pi<float>();
}

glm::vec3 soleil::materials::base::sample(const glm::vec3& hit_normal, const payload& pld) const
{
    return sample_hemisphere(hit_normal);
}
