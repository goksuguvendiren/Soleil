//
// Created by goksu on 6/7/19.
//

#include <vector>
#include <thread>
#include "mc_integrator.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "material.hpp"
#include "integrator.hpp"

inline void UpdateProgress(float progress)
{
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
};

glm::vec3 rtr::mc_integrator::render_pixel(const rtr::scene& scene, const rtr::camera& camera, const glm::vec3& pix_center,
                                           const rtr::image_plane& plane, const glm::vec3& right, const glm::vec3& below)
{
    // supersampling - jittered stratified
    constexpr int sq_sample_pp = 1;
    auto is_lens = std::bool_constant<true>();

    glm::vec3 color = {0, 0, 0};

    for (int k = 0; k < sq_sample_pp; ++k)
    {
        for (int m = 0; m < sq_sample_pp; ++m)
        {
            auto camera_pos = camera.position(); // random sample on the lens if not pinhole
            auto sub_pix_position = get_pixel_pos<sq_sample_pp>(pix_center, plane, camera, right, below, k, m, is_lens); // get the q
            auto ray = rtr::ray(camera_pos, sub_pix_position - camera_pos, 0, true);

            color += scene.trace(ray);
        }
    }

    return color / float(sq_sample_pp * sq_sample_pp);
}

void rtr::mc_integrator::render_line(const rtr::scene &scene, const glm::vec3& row_begin, int i)
{
    const auto& camera = scene.get_camera();
    rtr::image_plane plane(camera, width, height);

    auto right =  (1 / float(width))  * plane.right;
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
        threads.push_back(std::thread([i, &scene, pix_center, this, &below, &n, number_of_threads]
        {
        //            std::cerr << "thread " << i << " started!\n";
            for (int j = i; j < height; j += number_of_threads)
            {
                auto row_begin = pix_center + below * float(j);
                render_line(scene, row_begin, j);
                n++;
                // UpdateProgress(n / (float)height);
            }
        }));
    }

    for (auto& thread : threads) { thread.join(); }

}

std::vector<glm::vec3> rtr::mc_integrator::render(const rtr::scene& scene)
{
    sub_render(scene);
    return frame_buffer;
}
