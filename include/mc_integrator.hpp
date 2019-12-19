//
// Created by goksu on 6/7/19.
//

#pragma once

#include <glm/vec3.hpp>
#include "camera.hpp"
#include "camera.hpp"

namespace rtr
{
class ray;
class scene;
class mc_integrator
{
public:
    mc_integrator(unsigned int w, unsigned int h, int sq = 1) : width(w), height(h), sq_samples(sq)
    {
        frame_buffer.resize(width * height);
    }
    std::vector<glm::vec3> render(const rtr::scene& scene);
//
private:
    int sq_samples;
    unsigned int width;
    unsigned int height;

    std::vector<glm::vec3> frame_buffer;
    void sub_render(const rtr::scene& scene);
    void render_line(const rtr::scene &scene, const glm::vec3& row_begin, int i);
    glm::vec3 render_pixel(const rtr::scene& scene, const rtr::camera& camera, const glm::vec3& pix_center,
                           const rtr::image_plane& plane, const glm::vec3& right, const glm::vec3& below);
    glm::vec3 shade(const rtr::scene& scene, const rtr::ray& ray) const;
};
}
