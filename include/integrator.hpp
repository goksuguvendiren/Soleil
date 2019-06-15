#include <glm/vec3.hpp>

namespace rtr
{
template <int sq_sample_pp>
glm::vec3 get_pixel_pos(const glm::vec3& top_left, const rtr::image_plane& plane, const rtr::camera& camera,
                                        const glm::vec3& right, const glm::vec3& below, int u, int v, std::bool_constant<true>)
{
    auto random_u = get_random_float();
    auto random_v = get_random_float();

    auto x_offset = (random_u * (1.f / sq_sample_pp)) + ((float)u / sq_sample_pp);
    auto y_offset = (random_v * (1.f / sq_sample_pp)) + ((float)v / sq_sample_pp);

    // stratified sampling of the pixel. -> random p location on the film plane
    auto sample = top_left + right * x_offset + below * y_offset;

    auto line_through_lens_center = glm::normalize(camera.center() - sample); // direction of the ray from sample through the center of the lens
    auto point_on_plane = camera.center() + camera.focal_distance() * camera.view();

    auto plane_normal = camera.view();
    float nom = glm::dot(plane_normal, point_on_plane - sample);
    float denom = glm::dot(plane_normal, line_through_lens_center);

    auto t = nom / denom;

    return sample + t * line_through_lens_center; // returns the q
}

template <int sq_sample_pp>
glm::vec3 get_pixel_pos(const glm::vec3& top_left, const rtr::image_plane& plane, const rtr::camera& camera,
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
}
