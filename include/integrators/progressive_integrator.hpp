//
// Created by goksu on 6/7/19.
//

#pragma once

#include "camera.hpp"
#include "mc_integrator.hpp"

#include <glm/vec3.hpp>

namespace soleil
{
class ray;
class scene;
class progressive_integrator
{
public:
    progressive_integrator(unsigned int w, unsigned int h, int sq = 1)
        : m_mc_integrator(w, h, sq)
    {}

    std::vector<glm::vec3> render(const soleil::scene& scene);
    glm::vec3 render_pixel(const soleil::scene& scene, int i, int j);

private:
    soleil::mc_integrator m_mc_integrator;
};
} // namespace soleil
