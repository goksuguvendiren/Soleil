//
// Created by Göksu Güvendiren on 9.03.2020.
//

#include "lights/point.hpp"
#include "scene.hpp"

#include <payload.hpp>

namespace soleil::light
{
area::area(const soleil::primitives::quad& quad, const glm::vec3& intens) : m_intensity(intens), m_quad(quad)
{
    const auto& top_left = quad.a().position();
    const auto& right    = quad.b().position();
    const auto& bottom   = quad.d().position();

    m_position = top_left;
    m_u = right  - top_left;
    m_v = bottom - top_left;
}

std::optional<soleil::payload> area::hit(const soleil::ray& ray) const
{
    auto pld = m_quad.hit(ray);
    if (pld) pld->emission = m_intensity;

    return pld;
}

glm::vec3 area::random_sample_position() const
{
    auto random_u = get_random_float();
    auto random_v = get_random_float();

    return m_position + random_u * m_u + random_v * m_v;
}

std::pair<glm::vec3, glm::vec3> area::sample_li(const soleil::scene &scene, const soleil::payload& pld) const
{
    auto position = random_sample_position();
    auto difference = position - pld.hit_pos;

    float distance = glm::length(difference);
    auto light_dir = glm::normalize(difference);

    auto intensity = m_intensity;

    // check for visibility
    auto shadow_ray = soleil::ray(pld.hit_pos + (pld.hit_normal * 8e-2f), light_dir, pld.ray.rec_depth + 1, false);
    auto shadow_pld = scene.hit(shadow_ray);

    if (shadow_pld && (glm::distance(shadow_pld->hit_pos, pld.hit_pos) < glm::distance(position, pld.hit_pos)))
        intensity = glm::vec3(0, 0, 0);

    return {intensity / std::pow(distance, 2.f), light_dir};
}
} // namespac