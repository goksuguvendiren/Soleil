//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include "camera.hpp"
#include "dir_light.hpp"
#include "lights/directional.hpp"
#include "lights/point.hpp"
#include "payload.hpp"
#include "primitives/emissive_mesh.hpp"
#include "primitives/mesh.hpp"
#include "primitives/sphere.hpp"

#include <glm/vec3.hpp>
#include <lights/area.hpp>
#include <lights/directional.hpp>
#include <materials/texture.hpp>
#include <opencv2/opencv.hpp>
#include <optional>
#include <vector>

namespace soleil
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

    // objects:
    std::vector<soleil::primitives::sphere> spheres;
    std::vector<soleil::primitives::mesh> meshes;

    // light sources
    std::vector<soleil::light::point> lights;
    std::vector<soleil::light::directional> dir_lights;
    std::vector<soleil::light::area> area_lights;
    std::vector<std::unique_ptr<soleil::primitives::emissive_mesh>> mesh_lights;

    // materials
    std::vector<std::unique_ptr<soleil::materials::base>> materials;
    std::vector<std::unique_ptr<soleil::materials::texture>> textures;

    bool progressive_render = false;
    int samples_per_pixel;

    std::string output_file_name;
    std::string output_hdr_name;

    soleil::camera camera;

    unsigned int total_light_size() const { return lights.size() + dir_lights.size() + area_lights.size(); }
};

class scene
{
public:
    scene(scene_information sc_info)
        : information(std::move(sc_info))
    {
        soleil::aabb bounding_box;
        for (auto& m : information.meshes)
        {
            bounding_box = combine(bounding_box, m.bounding_box());
        }

        bounding_box = combine(bounding_box, information.camera.center());

        auto center = (bounding_box.max + bounding_box.min) / 2.f;

        information.materials.push_back(std::make_unique<soleil::materials::base>(glm::vec3(0.f, 0.f, 0.f), glm::vec3{0.1, 0.1, 0.1}, glm::vec3{0, 0, 0}, glm::vec3{0, 0, 0}, 0, 0));

        float constant = 3.f;
        bounding_sphere =
            soleil::primitives::sphere("scene bounding box", center, glm::length(center - bounding_box.max) * constant, information.materials.size() - 1);
    }

    int samples_per_pixel() const
    {
        return information.samples_per_pixel;
    }

    const soleil::camera& get_camera() const
    {
        return information.camera;
    }

    void set_camera(soleil::camera& cam)
    {
        information.camera = std::move(cam);
    }

    [[nodiscard]] const soleil::light::base& sample_light() const;

    [[nodiscard]] std::optional<soleil::payload> hit(const soleil::ray& ray) const;

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

    [[nodiscard]] const std::vector<soleil::light::point>& lights() const
    {
        return information.lights;
    }

    const std::vector<soleil::light::directional>& dir_lights() const
    {
        return information.dir_lights;
    }

    const std::vector<std::unique_ptr<soleil::primitives::emissive_mesh>>& mesh_lights() const
    {
    	return information.mesh_lights;
    }

    glm::vec3 trace(const soleil::ray& ray) const;
    glm::vec3 photon_trace(const soleil::ray& ray) const;
    glm::vec3 shadow_trace(const soleil::ray& ray, float light_distance) const;

    void print() const;

    std::string output_file_name() const { return information.output_file_name; }
    std::string output_hdr_name() const { return information.output_hdr_name; }

    const soleil::primitives::sphere& environment_sphere() const { return bounding_sphere; }
    int recursion_depth() const { return information.max_recursion_depth; }

    const std::unique_ptr<soleil::materials::base> &get_material(int index) const
    {
        assert(index >= 0 && " material index is negative!");
        return information.materials[index];
    }

    const std::unique_ptr<soleil::materials::texture> &get_texture(int index) const
    {
        assert(index >= 0 && " texture index is negative!");
        return information.textures[index];
    }

private:
    glm::vec3 shadow_trace(const soleil::ray& ray, float light_distance, int depth) const;

    soleil::primitives::sphere bounding_sphere;
    scene_information information;
};
} // namespace soleil
