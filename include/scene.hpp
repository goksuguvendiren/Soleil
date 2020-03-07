//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include "camera.hpp"
#include "dir_light.hpp"
#include "light.hpp"
#include "payload.hpp"
#include "primitives/emissive_mesh.hpp"
#include "primitives/mesh.hpp"
#include "primitives/sphere.hpp"
#include "scene_io.h"

#include <glm/vec3.hpp>
#include <materials/texture.hpp>
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

    std::vector<std::unique_ptr<rtr::primitives::emissive_mesh>> mesh_lights;
    std::vector<std::unique_ptr<rtr::materials::base>> materials;
    std::vector<std::unique_ptr<rtr::materials::texture>> textures;

    bool progressive_render = false;
    int samples_per_pixel;

    std::string output_file_name;
    std::string output_hdr_name;

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
            bounding_box = combine(bounding_box, m.bounding_box());
        }

        bounding_box = combine(bounding_box, information.camera.center());

        auto center = (bounding_box.max + bounding_box.min) / 2.f;

        information.materials.push_back(std::make_unique<rtr::materials::base>(glm::vec3(0.f, 0.f, 0.f), glm::vec3{0.1, 0.1, 0.1}, glm::vec3{0, 0, 0}, glm::vec3{0, 0, 0}, 0, 0));

        float constant = 3.f;
        bounding_sphere =
            rtr::primitives::sphere("scene bounding box", center, glm::length(center - bounding_box.max) * constant, information.materials.size() - 1);
    }

    int samples_per_pixel() const
    {
        return information.samples_per_pixel;
    }

    const rtr::camera& get_camera() const
    {
        return information.camera;
    }

    void set_camera(rtr::camera& cam)
    {
        information.camera = std::move(cam);
    }

    [[nodiscard]] const rtr::light* sample_light() const
    {
        auto random = get_random_float();
        auto index = 0;//int(random * information.lghts.size());
        return &(information.lghts[index]);
    }
//
//    rtr::primitives::emissive_mesh* sample_light() const
//    {
//        return mesh_lights()[0].get();
//    }

    std::optional<rtr::payload> hit(const rtr::ray& ray) const;

    template<class FnT>
    void for_each_light(FnT&& func) const
    {
//        std::for_each(lights().begin(), lights().end(), func);
//        std::for_each(dir_lights().begin(), dir_lights().end(), func);
        std::for_each(mesh_lights().begin(), mesh_lights().end(), [&func](auto&& light)
        {
            func(light.get());
        });
    }

    template<class FnT>
    void for_each_mesh_light(FnT&& func) const
    {
        std::for_each(mesh_lights().begin(), mesh_lights().end(), func);
    }

    [[nodiscard]] const std::vector<rtr::light>& lights() const
    {
        return information.lghts;
    }

    const std::vector<rtr::dir_light>& dir_lights() const
    {
        return information.dir_lghts;
    }

    const std::vector<std::unique_ptr<rtr::primitives::emissive_mesh>>& mesh_lights() const
    {
    	return information.mesh_lights;
    }

    glm::vec3 trace(const rtr::ray& ray) const;
    glm::vec3 photon_trace(const rtr::ray& ray) const;
    glm::vec3 shadow_trace(const rtr::ray& ray, float light_distance) const;

    void print() const;
    
    std::string output_file_name() const { return information.output_file_name; }

    const rtr::primitives::sphere& environment_sphere() const { return bounding_sphere; }
    int recursion_depth() const { return information.max_recursion_depth; }

    const std::unique_ptr<rtr::materials::base> &get_material(int index) const
    {
        assert(index >= 0 && " material index is negative!");
        return information.materials[index];
    }

    const std::unique_ptr<rtr::materials::texture> &get_texture(int index) const
    {
        assert(index >= 0 && " texture index is negative!");
        return information.textures[index];
    }

private:
    glm::vec3 shadow_trace(const rtr::ray& ray, float light_distance, int depth) const;

    rtr::primitives::sphere bounding_sphere;
    scene_information information;
};
} // namespace rtr
