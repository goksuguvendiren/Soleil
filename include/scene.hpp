//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include "camera.hpp"
#include "dir_light.hpp"
#include "light.hpp"
#include "payload.hpp"
#include "primitives/mesh.hpp"
#include "primitives/sphere.hpp"
#include "scene_io.h"

#include <glm/vec3.hpp>
#include <opencv2/opencv.hpp>
#include <optional>
#include <vector>

namespace rtr
{
class camera;
class ray;

// namespace primitives{
//     class sphere;
//     class mesh;
// }

struct scene_information
{
    scene_information() = default;
    glm::vec3 background_color;
    glm::vec3 ambient_light;
    float shadow_ray_epsilon = 1e-3;
    float intersection_test_epsilon = 1e-6;
    float max_recursion_depth = 5;

    std::vector<rtr::primitives::sphere> spheres;
    std::vector<rtr::primitives::mesh> meshes;
    std::vector<rtr::light> lghts;
    std::vector<rtr::dir_light> dir_lghts;

    rtr::camera camera;
};

class scene
{
public:
    scene(scene_information sc_info)
        : information(std::move(sc_info))
    {
        rtr::aabb bounding_box;
        for (auto& m : information.meshes)
        {
            std::cerr << m.bounding_box().min << ", " << m.bounding_box().max << '\n';
            bounding_box = combine(bounding_box, m.bounding_box());
        }

        bounding_box = combine(bounding_box, information.camera.center());

        auto center = (bounding_box.max + bounding_box.min) / 2.f;

        rtr::material m;
        m.diffuse = glm::vec3(2.f, 2.f, 2.f);

        float constant = 3.f;
        bounding_sphere =
            rtr::primitives::sphere("scene bounding box", center, glm::length(center - bounding_box.max) * constant, m);
    }

    const rtr::camera& get_camera() const
    {
        return information.camera;
    }
    void set_camera(rtr::camera& cam)
    {
        information.camera = std::move(cam);
    }

    std::optional<rtr::payload> hit(const rtr::ray& ray) const;

    template<class FnT>
    void for_each_light(FnT&& func) const
    {
        std::for_each(lights().begin(), lights().end(), func);
        std::for_each(dir_lights().begin(), dir_lights().end(), func);
    }

    const std::vector<rtr::light>& lights() const
    {
        return information.lghts;
    }
    const std::vector<rtr::dir_light>& dir_lights() const
    {
        return information.dir_lghts;
    }

    glm::vec3 trace(const rtr::ray& ray) const;
    glm::vec3 photon_trace(const rtr::ray& ray) const;
    glm::vec3 shadow_trace(const rtr::ray& ray, float light_distance) const;

    void print() const;

private:
    rtr::primitives::sphere bounding_sphere;
    scene_information information;
};
} // namespace rtr
