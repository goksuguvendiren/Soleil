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

    auto visualize_direction = [](const glm::vec3& dir) -> glm::vec3
    {
        return (dir + 1.f) * 0.5f;
    };

    // FIXME: quad loading bug, to be fixed
    if (glm::dot(ray.direction(), pld->hit_normal) > 0)
    {
        pld->hit_normal *= -1;
    }

    auto normal_visualized = visualize_direction(pld->hit_normal);

    const auto &material = scene.get_material(pld->material_idx);
    if (pld->emission)
    {
        return *pld->emission;
    }

    // direct lighting.
    const auto& light = scene.sample_light();
    auto [li, light_dir] = light.sample_li(scene, *pld);

    auto ldotn = glm::max(glm::dot(light_dir, pld->hit_normal), 0.f);
    auto bsdf = material->f(scene, *pld);

    auto L_direct = ldotn * li * bsdf * 2.f * glm::pi<float>();

    if (pld->ray.rec_depth >= 6)
    {
        // return direct lighting at the hit point
        return L_direct; // replace with russian roulette
    }

    // BRDF sampling:
    auto sample_direction = material->sample(pld->hit_normal, *pld);

    // incoming light from the random ray.
    auto reflection_ray = rtr::ray(pld->hit_pos + (pld->hit_normal * 7e-2f), sample_direction, pld->ray.rec_depth + 1, false);
    auto L_in = shade(scene, reflection_ray);

    auto cos_theta = glm::max(glm::dot(pld->hit_normal, sample_direction), 0.f);
    auto L_indirect = L_in * material->f(scene, *pld) * cos_theta * 2.f * glm::pi<float>();

    return (L_indirect + L_direct) / 2.f;
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
            color += pix_color;
        }
    }

    return color / float(sq_sample_pp * sq_sample_pp);
}

void rtr::mc_integrator::render_line(const rtr::scene& scene, const glm::vec3& row_begin, int i)
{
    const auto& camera = scene.get_camera();
    rtr::image_plane plane(camera, m_width, m_height);

    auto right = (1 / float(m_width)) * plane.right();
    auto below = -(1 / float(m_height)) * plane.up();

    glm::vec3 pix_center = row_begin;
    for (int j = 0; j < m_width; ++j)
    {
        pix_center += right;
        glm::vec3 color = {0.f, 0.f, 0.f};
        for (int i = 0; i < 1; ++i)
        {
            color += render_pixel(scene, camera, pix_center, plane, right, below);
        }

        frame_buffer[i * m_width + j] = color;// / float(scene.samples_per_pixel());
    }
}

void rtr::mc_integrator::sub_render(const rtr::scene& scene)
{
    const auto& camera = scene.get_camera();
    rtr::image_plane plane(camera, m_width, m_height);

    auto right = (1 / float(m_width)) * plane.right();
    auto below = -(1 / float(m_height)) * plane.up();

    auto pix_center = plane.top_left_position();

    auto number_of_threads = std::thread::hardware_concurrency();
    std::cerr << "Threads enabled! Running " << number_of_threads << " threads!\n";
    std::vector<std::thread> threads;
    int n = 0;
    for (int i = 0; i < number_of_threads; ++i)
    {
        threads.push_back(std::thread([i, &scene, pix_center, this, &below, &n, number_of_threads]
        {
            for (int j = i; j < m_height; j += number_of_threads)
            {
                auto row_begin = pix_center + below * float(j);
                render_line(scene, row_begin, j);
                n++;
                UpdateProgress(n / (float)m_height);
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
    rtr::image_plane plane(camera, m_width, m_height);

    auto right = (1 / float(m_width)) * plane.right();
    auto below = -(1 / float(m_height)) * plane.up();

    auto pix_center = plane.top_left_position();

    auto row_begin = pix_center + below * float(j);
    pix_center += float(j) * right;
    //    render_line(scene, row_begin, j);
    pix_center += right;
    auto color = render_pixel(scene, camera, pix_center, plane, right, below);

    frame_buffer[i * m_width + j] = color;

    return color;
}

std::vector<glm::vec3> rtr::mc_integrator::render(const rtr::scene& scene)
{
    sub_render(scene);

    return frame_buffer;
}
