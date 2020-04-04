//
// Created by Göksu Güvendiren on 9.03.2020.
//

#pragma once

#include "photon.hpp"

#include <glm/vec3.hpp>
#include "light.hpp"

namespace soleil
{
namespace light
{
class area : public base
{
public:
    area(const soleil::primitives::mesh& quad, const glm::vec3& intens);
    std::pair<glm::vec3, glm::vec3> sample_li(const soleil::scene& scene, const soleil::payload& pld) const;

    std::optional<soleil::payload> hit(const soleil::ray& ray) const;
private:
    glm::vec3 m_position;
    glm::vec3 m_u;
    glm::vec3 m_v;
    glm::vec3 m_intensity;

    soleil::primitives::mesh m_quad;

    glm::vec3 random_sample_position() const;
};
}
} // namespace