//
// Created by goksu on 6/7/19.
//

#pragma once

#include "camera.hpp"

#include <glm/vec3.hpp>

namespace soleil
{
class ray;
class scene;
class photon_map;
class photon_integrator
{
public:
    photon_integrator(unsigned int w, unsigned int h, int num_ph = 5'000)
        : width(w)
        , height(h)
        , num_photons(num_ph)
    {
        frame_buffer.resize(width * height);
    }
    std::vector<glm::vec3> render(const soleil::scene& scene);

private:
    int num_photons;
    unsigned int width;
    unsigned int height;

    std::vector<glm::vec3> frame_buffer;
    void sub_render(const soleil::scene& scene, const soleil::photon_map& p_map);
    void render_line(const soleil::scene& scene, const glm::vec3& row_begin, int i, const soleil::photon_map& p_map);
    glm::vec3 render_pixel(const soleil::scene& scene, const soleil::camera& camera, const soleil::photon_map& p_map,
                           const glm::vec3& pix_center, const soleil::image_plane& plane, const glm::vec3& right,
                           const glm::vec3& below);

    glm::vec3 render_ray(const soleil::scene& scene, const soleil::ray& ray, const soleil::photon_map& p_map);
};

template<int sq_sample_pp>
glm::vec3 get_pixel_pos(const glm::vec3& top_left, const soleil::image_plane& plane, const soleil::camera& camera,
                        const glm::vec3& right, const glm::vec3& below, int u, int v, std::bool_constant<true>)
{
    auto random_u = get_random_float();
    auto random_v = get_random_float();

    auto x_offset = (random_u * (1.f / sq_sample_pp)) + ((float)u / sq_sample_pp);
    auto y_offset = (random_v * (1.f / sq_sample_pp)) + ((float)v / sq_sample_pp);

    // stratified sampling of the pixel. -> random p location on the film plane
    auto sample = top_left + right * x_offset + below * y_offset;

    auto line_through_lens_center =
        glm::normalize(camera.center() - sample); // direction of the ray from sample through the center of the lens
    auto point_on_plane = camera.center() + camera.focal_distance() * camera.view();

    auto plane_normal = camera.view();
    float nom = glm::dot(plane_normal, point_on_plane - sample);
    float denom = glm::dot(plane_normal, line_through_lens_center);

    auto t = nom / denom;

    return sample + t * line_through_lens_center; // returns the q
}

template<int sq_sample_pp>
glm::vec3 get_pixel_pos(const glm::vec3& top_left, const soleil::image_plane& plane, const soleil::camera& camera,
                        const glm::vec3& right, const glm::vec3& below, int u, int v, std::bool_constant<false>)
{
    if constexpr (sq_sample_pp == 1)
    {
        return top_left + right * 0.5f + below * 0.5f;
    }

    auto random_u = get_random_float();
    auto random_v = get_random_float();

    auto x_offset = (random_u * (1.f / sq_sample_pp)) + ((float)u / sq_sample_pp);
    auto y_offset = (random_v * (1.f / sq_sample_pp)) + ((float)v / sq_sample_pp);

    auto sample = top_left + right * x_offset + below * y_offset;
    return sample;
}

} // namespace soleil
