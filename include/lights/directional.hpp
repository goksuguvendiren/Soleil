//
// Created by goksu on 3/8/20.
//

#pragma once

#include <glm/vec3.hpp>
#include "lights/light.hpp"
#include <utility>

namespace soleil::light
{
class scene;
class payload;
class directional : public base
{
public:
    directional(const glm::vec3& dir, const glm::vec3& intens);
    std::pair<glm::vec3, glm::vec3> sample_li(const soleil::scene& scene, const soleil::payload& pld) const;

private:
    glm::vec3 direction;
    glm::vec3 intensity;
};
}