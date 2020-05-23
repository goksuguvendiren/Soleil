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
#include <opencv2/opencv.hpp>
#include <optional>
#include <textures/sampler2D.hpp>
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

    glm::vec3 background_color = {0, 0, 0};
    glm::vec3 ambient_light;
    float shadow_ray_epsilon = 1e-3;
    float intersection_test_epsilon = 1e-6;
    int max_recursion_depth = 5;

    // objects:
    std::vector<soleil::primitives::sphere> spheres;
    std::vector<soleil::primitives::mesh> meshes;
    std::vector<soleil::primitives::quad> m_quads;

    // light sources
    std::vector<soleil::light::point> lights;
    std::vector<soleil::light::directional> dir_lights;
    std::vector<soleil::light::area> area_lights;
    std::vector<std::unique_ptr<soleil::primitives::emissive_mesh>> mesh_lights;

    // materials, and textures
    std::vector<std::unique_ptr<soleil::materials::base>> materials;
    std::vector<std::unique_ptr<soleil::textures::sampler2D>> textures;

    bool progressive_render = false;
    int samples_per_pixel;

    std::string output_file_name;
    std::string output_hdr_name;

    soleil::camera camera;

    std::optional<soleil::primitives::sphere> m_bounding_sphere;

    [[nodiscard]] unsigned int total_light_size() const { return lights.size() + dir_lights.size() + area_lights.size(); }
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
//
//        information.materials.push_back(std::make_unique<soleil::materials::base>(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3{0.1, 0.1, 0.1}, glm::vec3{0, 0, 0}, glm::vec3{0, 0, 0}, 0, 0));
//
        float constant = 3.f;
        if (information.m_bounding_sphere)
        information.m_bounding_sphere->radius = glm::length(center - bounding_box.max) * constant;
    }

    [[nodiscard]] int samples_per_pixel() const
    {
        return information.samples_per_pixel;
    }

    [[nodiscard]] const soleil::camera& get_camera() const
    {
        return information.camera;
    }

    void set_camera(soleil::camera cam)
    {
        information.camera = cam;
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

    [[nodiscard]] const std::vector<soleil::light::directional>& dir_lights() const
    {
        return information.dir_lights;
    }

    [[nodiscard]] const std::vector<std::unique_ptr<soleil::primitives::emissive_mesh>>& mesh_lights() const
    {
    	return information.mesh_lights;
    }

    glm::vec3 trace(const soleil::ray& ray) const;
    glm::vec3 photon_trace(const soleil::ray& ray) const;
    glm::vec3 shadow_trace(const soleil::ray& ray, float light_distance) const;

    void print() const;

    [[nodiscard]] std::string output_file_name() const { return information.output_file_name; }
    [[nodiscard]] std::string output_hdr_name() const { return information.output_hdr_name; }

    [[nodiscard]] std::optional<soleil::primitives::sphere> environment_sphere() const { return information.m_bounding_sphere; }
    [[nodiscard]] int recursion_depth() const { return information.max_recursion_depth; }

    [[nodiscard]] int total_light_size() const { return information.total_light_size(); }

    [[nodiscard]] glm::vec3 background_color() const { return information.background_color; }

    [[nodiscard]] const std::unique_ptr<soleil::materials::base> &get_material(int index) const
    {
        assert(index >= 0 && " material index is negative!");
        return information.materials[index];
    }

    [[nodiscard]] const std::unique_ptr<soleil::textures::sampler2D> &get_texture(int index) const
    {
        assert(index >= 0 && " texture index is negative!");
        return information.textures[index];
    }

private:
    glm::vec3 shadow_trace(const soleil::ray& ray, float light_distance, int depth) const;

    scene_information information;
};
} // namespace soleil
