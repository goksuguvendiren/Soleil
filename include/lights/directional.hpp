//
// Created by goksu on 3/8/20.
//

#pragma once

#include <glm/vec3.hpp>
#include "lights/light.hpp"
#include <utility>

namespace rtr::light
{
class scene;
class payload;
class directional : base
{
public:
    directional(const glm::vec3& dir, const glm::vec3& intens);
    std::pair<glm::vec3, glm::vec3> sample_li(const rtr::scene& scene, const rtr::payload& pld) const;

private:
    glm::vec3 direction;
    glm::vec3 intensity;
};
}