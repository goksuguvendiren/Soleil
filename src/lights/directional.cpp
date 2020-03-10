//
// Created by goksu on 3/8/20.
//

//
// Created by goksu on 3/8/20.
//

#include "lights/directional.hpp"
#include "scene.hpp"

#include <payload.hpp>

namespace rtr::light
{
directional::directional(const glm::vec3& dir, const glm::vec3& intens) : direction(dir), intensity(intens)
{}

std::pair<glm::vec3, glm::vec3> directional::sample_li(const rtr::scene &scene, const rtr::payload& pld) const
{
    auto intens = intensity;
    auto shadow_direction = -direction;

    // check for visibility
    auto shadow_ray = rtr::ray(pld.hit_pos + (pld.hit_normal * 1e-4f), shadow_direction, pld.ray.rec_depth + 1, false);
    auto shadow_pld = scene.hit(shadow_ray);

    if (shadow_pld)
        intens = glm::vec3(0, 0, 0);

    return {intens, -glm::normalize(direction)};

}
} // namespace rtr::lights