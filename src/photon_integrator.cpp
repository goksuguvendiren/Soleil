//
// Created by goksu on 6/7/19.
//

#include <vector>
#include <thread>
#include "photon_integrator.hpp"
#include "photon.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "material.hpp"
#include "photon_map.hpp"
#include "photon.hpp"
//#include "integrator.hpp"

void trace_photons(const rtr::scene& scene, const rtr::photon& photon, std::vector<rtr::photon>& hit_photons)
{
//    auto photon_ray = rtr::ray(photon.origin(), photon.direction(), 0, false);
//    auto hit = scene.hit(photon_ray);
//    std::cerr << "Hit photon! \n";

//    if (!hit) return;
//
//    // there's a hit, save the photon in the photon map
//    hit_photons.emplace_back(photon.power(), hit->hit_pos, photon_ray.direction());
//
//    // decide the photon's fate with russian roulette
//    rtr::PathType decision = hit->material->russian_roulette();
//
//    if(decision == rtr::PathType::Absorbed)
//    {
//        std::cerr << "Photon absorbed\n";
//        return;
//    }
//
//     auto direction = hit->material->sample(hit->hit_normal);
//
//////  TODO: update the power now!
//    rtr::photon new_photon(photon.power() * hit->material->diffuse, hit->hit_pos, direction);
//    trace_photons(scene, new_photon, hit_photons);
}

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

glm::vec3 rtr::photon_integrator::render_pixel(const rtr::scene& scene, const rtr::camera& camera, const rtr::photon_map& p_map, 
                                        const glm::vec3& pix_center, const rtr::image_plane& plane, const glm::vec3& right, const glm::vec3& below)
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
            auto sub_pix_position = rtr::get_pixel_pos<sq_sample_pp>(pix_center, plane, camera, right, below, k, m, is_lens); // get the q
            auto ray = rtr::ray(camera_pos, sub_pix_position - camera_pos, 0, true);

            auto payload = scene.hit(ray);
            if (!payload) continue;

            // direct illumination :
            color += payload->material->shade(scene, *payload);

            // std::cerr << "color : " << color << " - ";

            auto near_photons = p_map.search(payload->hit_pos, 5);
            // std::cerr << near_photons.size() << '\n';
            // std::cerr << "direct : " << color << '\n';


            auto indir_color = std::accumulate(near_photons.begin(), near_photons.end(), glm::vec3(0), [](auto& a, auto& p) { return a + p.power(); });
            // std::cerr << " indirect : " << glm::vec3(indir_color / float(near_photons.size())) << " - " ;
            
            if (near_photons.size() > 0)
                color += (indir_color / float(near_photons.size()));
            // std::cerr << " output color : " << color << '\n';
            // std::cerr << "indirect : "  << glm::vec3(indir_color / float(near_photons.size())) << '\n';
            
            color.x = std::clamp(color.x, 0.f, 1.f);
            color.y = std::clamp(color.y, 0.f, 1.f);
            color.z = std::clamp(color.z, 0.f, 1.f);

            return color;// / float(near_photons.size());
        }
    }
    
    return color / float(sq_sample_pp * sq_sample_pp);
}

void rtr::photon_integrator::render_line(const rtr::scene &scene, const glm::vec3& row_begin, int i, const rtr::photon_map& p_map)
{
    const auto& camera = scene.get_camera();
    rtr::image_plane plane(camera, width, height);
    
    auto right =  (1 / float(width))  * plane.right;
    auto below = -(1 / float(height)) * plane.up;
    
    glm::vec3 pix_center = row_begin;
    for (int j = 0; j < width; ++j)
    {
        pix_center += right;
        auto color = render_pixel(scene, camera, p_map, pix_center, plane, right, below);

        frame_buffer[i * width + j] = color;
    }
}

void rtr::photon_integrator::sub_render(const rtr::scene& scene, const rtr::photon_map& p_map)
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
        threads.push_back(std::thread([i, &scene, pix_center, this, &below, &n, number_of_threads, &p_map]
        {
        //            std::cerr << "thread " << i << " started!\n";
            for (int j = i; j < height; j += number_of_threads)
            {
                auto row_begin = pix_center + below * float(j);
                render_line(scene, row_begin, j, p_map);
                n++;
                UpdateProgress(n / (float)height);
            }
        }));
    }

    for (auto& thread : threads) { thread.join(); }

}

std::vector<glm::vec3> rtr::photon_integrator::render(const rtr::scene& scene)
{
    // Phase 1 for photon mapping:
    // Iterate through all the lights, and shoot photons in random directions.
    std::vector<rtr::photon> emitted_photons;
    scene.for_each_light([this, &emitted_photons](auto light)
    {
        auto new_photons = light.distribute_photons(num_photons);
        emitted_photons.insert(emitted_photons.end(), std::make_move_iterator(new_photons.begin()), std::make_move_iterator(new_photons.end()));
    });
    
    // actual photon mapping. Trace all the photons through their paths and
    // save their positions and directions.
    std::vector<rtr::photon> hit_photons;
    for (auto& photon : emitted_photons)
    {
        trace_photons(scene, photon, hit_photons);
    }
    // std::cout << hit_photons.size() << '\n';

    rtr::photon_map p_map(hit_photons);
    sub_render(scene, p_map);

    return frame_buffer;
}
