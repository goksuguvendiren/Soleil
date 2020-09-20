#include "payload.hpp"
#include "scene.hpp"

#include <bsdfs/lambert.hpp>
#include <materials/matte.hpp>

glm::vec3 soleil::bsdfs::lambert::f(const soleil::scene& scene, const soleil::payload& pld, const glm::vec3& light_direction) const
{
    glm::vec3 albedo = m_albedo;
    if (m_texture)
        albedo = m_texture->get_sample(pld.texture_coords);

    return albedo / glm::pi<float>();
}

glm::vec3 soleil::bsdfs::lambert::sample(const glm::vec3& hit_normal, const payload& pld) const
{
    return sample_hemisphere(hit_normal);
}
