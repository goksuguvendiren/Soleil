//
// Created by Göksu Güvendiren on 12.02.2020.
//

#include "integrators/mc_integrator.hpp"

#include "camera.hpp"
#include "integrator.hpp"
#include "materials/base.hpp"
#include "scene.hpp"

#include <renderer.hpp>
#include <thread>
#include <vector>

glm::vec3 rtr::mc_integrator::shade(const rtr::scene& scene, const rtr::ray& ray) const
{
    auto pld = scene.hit(ray);

    if (!pld)
    {
        return glm::vec3(0,0,0);
    }

    // FIXME: quad loading bug, to be fixed
//    if (glm::dot(ray.direction(), pld->hit_normal) > 0)
//    {
//        pld->hit_normal *= -1;
//    }

    const auto &material = scene.get_material(pld->material_idx);
    if (material->is_emissive())
    {
        return material->f(scene, *pld);
    }

    if (pld->ray.rec_depth >= 4)
        return material->f(scene, *pld); // replace with russian roulette

    return (pld->hit_normal + 1.f) * 0.5f;

    // BRDF sampling:
    auto sample_direction = material->sample(pld->hit_normal, *pld);

    // incoming light from the random ray.
    auto reflection_ray = rtr::ray(pld->hit_pos + (pld->hit_normal * 7e-2f), sample_direction, pld->ray.rec_depth + 1, false);
    auto L_in = shade(scene, reflection_ray);

    auto cos_theta = glm::max(glm::dot(pld->hit_normal, sample_direction), 0.f);
    auto L_indirect = L_in * material->f(scene, *pld) * cos_theta * 2.f * glm::pi<float>();

    // direct lighting.
    auto sample_light = scene.sample_light();
    auto light_position = sample_light->position();
    auto light_dir = glm::normalize(light_position - pld->hit_pos);

    // check for visibility
    auto shadow_ray = rtr::ray(pld->hit_pos + (pld->hit_normal * 8e-2f), light_dir, pld->ray.rec_depth + 1, false);
    auto shadow_pld = scene.hit(shadow_ray);

    if (shadow_pld && (glm::distance(shadow_pld->hit_pos, pld->hit_pos) < glm::distance(light_position, pld->hit_pos)))
//        return L_indirect;
        return glm::vec3(0,0,0);

    auto ldotn = glm::max(glm::dot(light_dir, pld->hit_normal), 0.f);
    auto bsdf = material->f(scene, *pld);

    auto L_direct = ldotn * sample_light->intensity() * sample_light->attenuate(light_position, pld->hit_pos) * bsdf;

    return L_direct; // (L_indirect + L_direct) / 2.f;
}

glm::vec3 rtr::mc_integrator::render_pixel(const rtr::scene& scene, const rtr::camera& camera,
                                                    const glm::vec3& pix_center, const rtr::image_plane& plane,
                                                    const glm::vec3& right, const glm::vec3& below)
{
    // supersampling - jittered stratified
    constexpr int sq_sample_pp = 1;
    auto is_lens = std::bool_constant<false>();

    glm::vec3 color = {0, 0, 0};

    for (int k = 0; k < sq_sample_pp; ++k)
    {
        for (int m = 0; m < sq_sample_pp; ++m)
        {
            auto camera_pos = camera.position(); // random sample on the lens if not pinhole
            auto sub_pix_position =
                get_pixel_pos<sq_sample_pp>(pix_center, plane, camera, right, below, k, m, is_lens); // get the q
            auto ray = rtr::ray(camera_pos, sub_pix_position - camera_pos, 0, true);

            auto pix_color = shade(scene, ray);
//            std::cerr << pix_color << '\n';
            color += pix_color;
        }
    }

    return color / float(sq_sample_pp * sq_sample_pp);
}

void rtr::mc_integrator::render_line(const rtr::scene& scene, const glm::vec3& row_begin, int i)
{
    const auto& camera = scene.get_camera();
    rtr::image_plane plane(camera, width, height);

    auto right = (1 / float(width)) * plane.right();
    auto below = -(1 / float(height)) * plane.up();

    glm::vec3 pix_center = row_begin;
    for (int j = 0; j < width; ++j)
    {
        pix_center += right;
        glm::vec3 color = {0.f, 0.f, 0.f};
        for (int i = 0; i < 1; ++i)
        {
            color += render_pixel(scene, camera, pix_center, plane, right, below);
        }

        frame_buffer[i * width + j] = color;// / float(scene.samples_per_pixel());
    }
}

void rtr::mc_integrator::sub_render(const rtr::scene& scene)
{
    const auto& camera = scene.get_camera();
    rtr::image_plane plane(camera, width, height);

    auto right = (1 / float(width)) * plane.right();
    auto below = -(1 / float(height)) * plane.up();

    auto pix_center = plane.top_left_position();

    auto number_of_threads = std::thread::hardware_concurrency();
    std::cerr << "Threads enabled! Running " << number_of_threads << " threads!\n";
    std::vector<std::thread> threads;
    int n = 0;
    for (int i = 0; i < number_of_threads; ++i)
    {
        threads.push_back(std::thread([i, &scene, pix_center, this, &below, &n, number_of_threads]
        {
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

glm::vec3 rtr::mc_integrator::render_pixel(const rtr::scene& scene, int i, int j)
{
    const auto& camera = scene.get_camera();
    rtr::image_plane plane(camera, width, height);

    auto right = (1 / float(width)) * plane.right();
    auto below = -(1 / float(height)) * plane.up();

    auto pix_center = plane.top_left_position();

    auto row_begin = pix_center + below * float(j);
    pix_center += float(j) * right;
    //    render_line(scene, row_begin, j);
    pix_center += right;
    auto color = render_pixel(scene, camera, pix_center, plane, right, below);

    frame_buffer[i * width + j] = color;

    return color;
}

std::vector<glm::vec3> rtr::mc_integrator::render(const rtr::scene& scene)
{
    sub_render(scene);

    return frame_buffer;
}
