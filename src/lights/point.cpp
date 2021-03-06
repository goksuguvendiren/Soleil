//
// Created by goksu on 3/8/20.
//

#include "lights/point.hpp"
#include "scene.hpp"

#include <payload.hpp>

namespace soleil::light
{
point::point(const glm::vec3& poss, const glm::vec3& intens) : m_position(poss), m_intensity(intens)
{}

std::pair<glm::vec3, glm::vec3> point::sample_li(const soleil::scene &scene, const soleil::payload& pld) const
{
    auto difference = m_position - pld.hit_pos;

    float distance = glm::length(difference);
    auto light_dir = glm::normalize(difference);

    // check for visibility
    auto shadow_ray = soleil::ray(pld.hit_pos + (pld.hit_normal * 8e-2f), light_dir, pld.ray.rec_depth + 1, false);
    auto shadow_pld = scene.hit(shadow_ray);

    if (shadow_pld && (glm::distance(shadow_pld->hit_pos, pld.hit_pos) < glm::distance(m_position, pld.hit_pos)))
//        return L_indirect;
        return {glm::vec3(0,0,0), light_dir};

    return {m_intensity / std::pow(distance, 2.f), light_dir};

}
} // namespace soleil::lights