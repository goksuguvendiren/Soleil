//
// Created by Göksu Güvendiren on 12.02.2020.
//

#pragma once


#include "camera.hpp"

#include <glm/vec3.hpp>

namespace rtr
{
class ray;
class scene;
class mc_integrator
{
public:
    mc_integrator(unsigned int w, unsigned int h, int sq = 1)
        : m_width(w)
        , m_height(h)
        , sq_samples(sq)
    {
        frame_buffer.resize(m_width * m_height);
    }

    auto width() const { return m_width; }
    auto height() const { return m_height; }

    std::vector<glm::vec3> render(const rtr::scene& scene);
    glm::vec3 render_pixel(const rtr::scene& scene, int i, int j);
    [[nodiscard]] const std::vector<glm::vec3>& get_frame_buffer() const { return frame_buffer; }

private:
    int sq_samples;
    unsigned int m_width;
    unsigned int m_height;

    std::vector<glm::vec3> frame_buffer;

    void sub_render(const rtr::scene& scene);
    void render_line(const rtr::scene& scene, const glm::vec3& row_begin, int i);
    glm::vec3 render_pixel(const rtr::scene& scene, const rtr::camera& camera, const glm::vec3& pix_center,
                           const rtr::image_plane& plane, const glm::vec3& right, const glm::vec3& below);
    [[nodiscard]] glm::vec3 shade(const rtr::scene& scene, const rtr::ray& ray) const;
};
} // namespace rtr
