//
// Created by goksu on 6/7/19.
//

#include "mc_integrator.hpp"

#include "camera.hpp"
#include "integrator.hpp"
#include "materials/base.hpp"
#include "scene.hpp"

#include <thread>
#include <vector>

inline void UpdateProgress(float progress)
{
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < pos)
            std::cout << "=";
        else if (i == pos)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
};

glm::vec3 rtr::mc_integrator::shade(const rtr::scene& scene, const rtr::ray& ray) const
{
    auto pld = scene.hit(ray);

    if (!pld)
    {
//        return color;
        // miss: then hit the bounding box.
        auto pld = scene.environment_sphere().hit(ray);
        if (pld)
        {
            const auto &material = scene.get_material(pld->material_idx);
            auto res_color = material->shade(scene, *pld);
            return res_color;
        }
        // assert(false && "Should always hit the bounding sphere");
        return glm::vec3(0,0,0);
    }

    const auto &material = scene.get_material(pld->material_idx);
    if (material->is_emissive())
    {
        return material->shade(scene, *pld);
    }
    if (pld->ray.rec_depth >= 8)
        return material->shade(scene, *pld); // replace with russian roulette

    // return glm::vec3(1.0f);
    // return ((pld->hit_normal + 1.0f) / 2.f);
    // return material->diffuse;

    //    auto sample_direction = sample_hemisphere(pld->hit_normal);
    auto sample_direction = material->sample(pld->hit_normal, *pld);
    auto reflection_ray = rtr::ray(pld->hit_pos + (sample_direction * 1e-3f), sample_direction, pld->ray.rec_depth + 1, false);

    auto material_color = material->shade(scene, *pld);
    auto irradiance = shade(scene, reflection_ray);

    // std::cout << material_color << " * " << irradiance << '\n';

    return material_color * irradiance;

    //
    //    color = pld->material->shade(*this, *pld);
    //
    //    // Reflection :
    //    if (pld->material->specular.x > 0.f || pld->material->specular.y > 0.f || pld->material->specular.z > 0.f)
    //    {
    //        auto reflection_direction = reflect(ray.direction(), pld->hit_normal);
    //        rtr::ray reflected_ray(pld->hit_pos + (reflection_direction * 1e-3f), reflection_direction,
    //        pld->ray.rec_depth + 1, false); color += pld->material->specular * trace(reflected_ray);
    //    }
    //
    //    // Refraction
    //    if (pld->material->trans > 0.f)
    //    {
    //        auto eta_1 = 1.f;
    //        auto eta_2 = 1.5f;
    //
    //        auto refraction_direction = refract(ray.direction(), pld->hit_normal, eta_2 / eta_1);
    //        if (glm::length(refraction_direction) > 0.1)
    //        {
    //            rtr::ray refracted_ray(pld->hit_pos + (refraction_direction * 1e-3f), refraction_direction,
    //            pld->ray.rec_depth + 1, false); color += pld->material->trans * trace(refracted_ray);
    //        }
    //    }
}

glm::vec3 rtr::mc_integrator::render_pixel(const rtr::scene& scene, const rtr::camera& camera,
                                           const glm::vec3& pix_center, const rtr::image_plane& plane,
                                           const glm::vec3& right, const glm::vec3& below)
{
    // supersampling - jittered stratified
    constexpr int sq_sample_pp = 4;
    auto is_lens = std::bool_constant<true>();

    glm::vec3 color = {0, 0, 0};

    for (int k = 0; k < sq_sample_pp; ++k)
    {
        for (int m = 0; m < sq_sample_pp; ++m)
        {
            auto camera_pos = camera.position(); // random sample on the lens if not pinhole
            auto sub_pix_position =
                get_pixel_pos<sq_sample_pp>(pix_center, plane, camera, right, below, k, m, is_lens); // get the q
            auto ray = rtr::ray(camera_pos, sub_pix_position - camera_pos, 0, true);

            // color += scene.trace(ray);

            auto pix_color = shade(scene, ray);
            //dstc::rer< pix_color << '\n';
            color += pix_color;
        }
    }

    return color / float(sq_sample_pp * sq_sample_pp);
}

void rtr::mc_integrator::render_line(const rtr::scene& scene, const glm::vec3& row_begin, int i)
{
    const auto& camera = scene.get_camera();
    rtr::image_plane plane(camera, width, height);

    auto right = (1 / float(width)) * plane.right;
    auto below = -(1 / float(height)) * plane.up;

    glm::vec3 pix_center = row_begin;
    for (int j = 0; j < width; ++j)
    {
        pix_center += right;
        auto color = render_pixel(scene, camera, pix_center, plane, right, below);

        frame_buffer[i * width + j] = color;
    }
}

void rtr::mc_integrator::sub_render(const rtr::scene& scene)
{
    const auto& camera = scene.get_camera();
    rtr::image_plane plane(camera, width, height);

    auto right = (1 / float(width)) * plane.right;
    auto below = -(1 / float(height)) * plane.up;

    auto pix_center = plane.top_left_position();

    // cv::namedWindow("window");
    // cv::setMouseCallback("window", CallBackFunc, NULL);

    int number_of_threads = std::thread::hardware_concurrency();
    std::cerr << "Threads enabled! Running " << number_of_threads << " threads!\n";
    std::vector<std::thread> threads;
    int n = 0;
    for (int i = 0; i < number_of_threads; ++i)
    {
        threads.push_back(std::thread([i, &scene, pix_center, this, &below, &n, number_of_threads] {
            //            std::cerr << "thread " << i << " started!\n";
            for (int j = i; j < height; j += number_of_threads)
            {
                auto row_begin = pix_center + below * float(j);
                render_line(scene, row_begin, j);
                n++;
                UpdateProgress(n / (float)height);
            }
        }));
    }

    for (auto& thread : threads)
    {
        thread.join();
    }
}

std::vector<glm::vec3> rtr::mc_integrator::render(const rtr::scene& scene)
{
    sub_render(scene);
    return frame_buffer;
}
