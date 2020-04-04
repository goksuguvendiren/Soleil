//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include "photon.hpp"

#include <glm/vec3.hpp>
#include "light.hpp"

namespace soleil
{
namespace light
{
class point : public base
{
public:
    point(const glm::vec3& poss, const glm::vec3& intens);
    std::pair<glm::vec3, glm::vec3> sample_li(const soleil::scene& scene, const soleil::payload& pld) const;

private:
    glm::vec3 position;
    glm::vec3 intensity;
};
}
} // namespace soleil