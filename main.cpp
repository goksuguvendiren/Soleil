#include "integrators/progressive_integrator.hpp"
#include "photon_integrator.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "scene_loaders.hpp"

#include <integrators/mc_integrator.hpp>
#include <iostream>
#include <scene_io.h>

int main(int argc, const char** argv)
{
    auto begin = std::chrono::system_clock::now();

//        std::string scene_path = "../Scenes/obj/bunny/bunny.obj";
    std::string scene_path = "../Scenes/Tungsten/cornell-box/scene_without_cubes.json";
    //    std::string scene_path = "../../Scenes/obj/CornellBox/CornellBox-Original.obj";
    std::cerr << scene_path << '\n';
    bool pinhole_camera = true;
    float image_plane_distance = 1.f;
    float lens_width = 1.f;
    float focal_length = 1.f;
    if (argc > 1)
    {
        scene_path = std::string(argv[1]);
        if (argc == 4)
        {
            pinhole_camera = false;
            image_plane_distance = std::stof(std::string(argv[2]));
            lens_width = std::stof(std::string(argv[3]));
        }
        else if (argc == 5)
        {
            pinhole_camera = false;
            image_plane_distance = std::stof(std::string(argv[2]));
            lens_width = std::stof(std::string(argv[3]));
            focal_length = std::stof(std::string(argv[4]));
        }
    }

    rtr::scene scene = rtr::loaders::load(scene_path);

    // scene.print();

    auto width = scene.get_camera().width;
    auto height = scene.get_camera().height;

    auto end = std::chrono::system_clock::now();
    std::cerr << "Scene loading took : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << " millisecs.";

//    rtr::renderer<rtr::mc_integrator> r(width, height);
    rtr::progressive_integrator r(width, height);

    begin = std::chrono::system_clock::now();
    auto output_buffer = r.render(scene);
    end = std::chrono::system_clock::now();

    auto debug_color = r.render_pixel(scene, 100, 100);

    std::cout << "Rendering took : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << " millisecs.";

    cv::Mat image(height, width, CV_32FC3, output_buffer.data());
    cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
    cv::imshow(scene.output_file_name(), image);
    cv::waitKey(0);
    cv::imwrite(scene.output_file_name() + "_output.exr", image);

    return 0;
}
