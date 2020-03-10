//
// Created by goksu on 3/8/20.
//

#pragma once

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <payload.hpp>
#include <photon.hpp>
#include <utils.hpp>
namespace rtr
{
class scene;
namespace light
{
class base
{
public:
    virtual std::pair<glm::vec3, glm::vec3> sample_li(const rtr::scene& scene, const rtr::payload& pld) const = 0;

//    std::vector<rtr::photon> distribute_photons(int num_photons)
//    {
//        std::vector<photon> photons;
//
//        for (int i = 0; i < num_photons; ++i)
//        {
//            bool found_photon = false;
//            float x, y, z;
//            while (!found_photon)
//            {
//                x = get_random_float(-1, 1);
//                y = get_random_float(-1, 1);
//                z = get_random_float(-1, 1);
//
//                found_photon = (x * x + y * y + z * z <= 1); // rejection sampling the sphere
//            }
//
//            photons.emplace_back((pow * color) / float(num_photons), pos, glm::normalize(glm::vec3{x, y, z}));
//        }
//
//        return photons;
//    }

    [[nodiscard]] glm::vec3 intensity() const
    {
        return pow * color;
    }

private:
    glm::vec3 pos;
    glm::vec3 color;
    float pow;
};
}
}