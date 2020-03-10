//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include "utils.hpp"

#include <glm/glm.hpp>

namespace rtr
{
struct degrees
{
    float degree;
};

struct radians
{
    float radian;
};

class camera
{
public:
    camera() = default;
    camera(const glm::vec3& pos, const glm::vec3& view, const glm::vec3& up, rtr::radians fov, int w, int h,
           bool pin = true)
        : eye_pos(pos)
        , view_dir(view)
        , up_dir(glm::normalize(up))
        , vert_fov(fov.radian)
        , pinhole(pin)
        , width(w)
        , height(h)
    {
        if (pin)
        {
            // if the camera model is a pinhole camera, then the distance to the image plane is
            // the focal distance
//            image_plane_dist = focal_dist;
        } else
        {
            // TODO : make this variable
            lens_width = 0.1;
        }

        right_dir = glm::normalize(glm::cross(view_dir, up_dir));
        up_dir = glm::normalize(glm::cross(right_dir, view_dir));
    }

    auto up() const
    {
        return up_dir;
    }

    auto right() const
    {
        return right_dir;
    }

    auto view() const
    {
        return glm::normalize(view_dir);
    }

    auto center() const
    {
        return eye_pos;
    }

    glm::vec3 position() const
    {
        return pinhole ? eye_pos : point_sample_lens();
    }

    float focal_distance() const
    {
        auto focal_distance = (height / 2.f) / glm::tan(vert_fov / 2.f);
        return focal_distance;
    }

    auto fov() const
    {
        return vert_fov;
    }

    auto is_pinhole() const
    {
        return pinhole;
    }

    bool pinhole;
    float lens_width;

    int width;
    int height;

private:
    glm::vec3 eye_pos;
    glm::vec3 view_dir;
    glm::vec3 up_dir;
    glm::vec3 right_dir;

    float vert_fov;

    glm::vec3 point_sample_lens() const
    {
        auto u = get_random_float(-1, 1);
        auto v = get_random_float(-1, 1);

        return eye_pos + u * right_dir * lens_width / 2.f + v * up_dir * lens_width / 2.f;
    }
};

class image_plane
{
public:
    image_plane(const camera& cam, unsigned int w, unsigned int h)
    {
        m_up = (h / 2.f) * cam.up();
        m_right = (w / 2.f) * cam.right();

        auto focal_dist = cam.focal_distance();

        glm::vec3 center;
        if (cam.is_pinhole())
        {
            center = cam.center() + focal_dist * cam.view();
        } else
        {
            // The camera has a lens, in this case the image plane is behind the camera
            center = cam.center() - focal_dist * cam.view();
        }
        m_top_left = center + m_up - m_right; // CAN BE DIFFERENT! Check!

        m_up *= 2;
        m_right *= 2;
    }

    [[nodiscard]] glm::vec3 top_left_position() const
    {
        return m_top_left;
    }

    [[nodiscard]] glm::vec3 right() const
    {
        return m_right;
    }

    [[nodiscard]] glm::vec3 up() const
    {
        return m_up;
    }

private:
    glm::vec3 m_top_left;
    glm::vec3 m_right;
    glm::vec3 m_up;

};
} // namespace rtr
