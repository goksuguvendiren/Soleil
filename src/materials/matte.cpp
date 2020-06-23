//
// Created by goksu on 6/22/20.
//

#include <materials/matte.hpp>
#include <payload.hpp>
namespace soleil::materials
{
glm::vec3 matte::f(const scene& scene, const payload& pld, const glm::vec3& light_direction) const
{
    return std::visit([&](auto& bsdf)
    {
        return bsdf.f(scene, pld, light_direction);
    }, m_bsdf);
}

glm::vec3 matte::sample(const glm::vec3 &hit_normal, const payload &pld) const
{
    return sample_hemisphere(hit_normal);
}
}