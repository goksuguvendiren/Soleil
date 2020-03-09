//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include "photon.hpp"

#include <glm/vec3.hpp>

namespace rtr
{
// point light
class light
{
public:
    light(const glm::vec3& poss, const glm::vec3& col)
        : pos(poss)
        , color(col)
        , pow(1)
    {}
    glm::vec3 pos;
    glm::vec3 color;

    glm::vec3 position() const
    {
        return pos;
    }

    glm::vec3 direction(const glm::vec3& hit_pos) const
    {
        return glm::normalize(pos - hit_pos);
    }

    float distance(const glm::vec3& hit_pos) const
    {
        return glm::length(pos - hit_pos);
    }

    float attenuate(const glm::vec3& light_pos, const glm::vec3& hit_pos) const
    {
        auto distance = glm::length(light_pos - hit_pos);
        float c_1 = 0.25;
        float c_2 = 0.1;
        float c_3 = 0.01;

        auto attenuation = 1.f / float(c_1 + c_2 * distance + c_3 * distance * distance);
        return std::min(1.f, attenuation);
    }

    std::vector<rtr::photon> distribute_photons(int num_photons)
    {
        std::vector<photon> photons;

        for (int i = 0; i < num_photons; ++i)
        {
            bool found_photon = false;
            float x, y, z;
            while (!found_photon)
            {
                x = get_random_float(-1, 1);
                y = get_random_float(-1, 1);
                z = get_random_float(-1, 1);

                found_photon = (x * x + y * y + z * z <= 1); // rejection sampling the sphere
            }

            photons.emplace_back((pow * color) / float(num_photons), pos, glm::normalize(glm::vec3{x, y, z}));
        }

        return photons;
    }

    [[nodiscard]] glm::vec3 intensity() const
    {
        return pow * color;
    }

private:
    float pow;
};
} // namespace rtr