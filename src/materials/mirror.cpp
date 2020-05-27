//
// Created by goksu on 5/23/20.
//

#include "materials/mirror.hpp"

#include <payload.hpp>

namespace soleil::materials
{
glm::vec3 mirror::f(const scene &scene, const payload &pld) const
{
    return glm::vec3(1.f);
}

glm::vec3 mirror::sample(const glm::vec3 &hit_normal, const payload &pld) const
{
//    return hit_normal;
    auto w_o = -pld.ray.direction();
    auto cos_theta = glm::dot(hit_normal, w_o);

    assert(cos_theta > 0);

    auto reflected_ray = - w_o + 2 * cos_theta * hit_normal;
    assert(glm::dot(reflected_ray, hit_normal) >= 0);
    return reflected_ray;
}
}