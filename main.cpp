#include "mc_integrator.hpp"
#include "photon_integrator.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "scene_loaders.hpp"

#include <iostream>
#include <scene_io.h>

int main(int argc, const char** argv)
{
    auto begin = std::chrono::system_clock::now();

//        std::string scene_path = "../Scenes/obj/bunny/bunny.obj";
    std::string scene_path = "../Scenes/Tungsten/cornell-box/scene.json";
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
    rtr::mc_integrator r(width, height);
    std::vector<glm::vec3> accum_buffer;
    std::vector<glm::vec3> result_buffer;
    std::vector<glm::vec3> output_buffer;

    accum_buffer.resize(width * height);
    result_buffer.resize(width * height);

    int n_frames = 0;

    int key = 0;
    while (key != 27)
    {
        begin = std::chrono::system_clock::now();
        output_buffer = r.render(scene);

        auto out_color = r.render_pixel(scene, 100, 100);
        end = std::chrono::system_clock::now();

        std::cout << "Rendering took : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
                  << " millisecs.";

        for (int i = 0; i < output_buffer.size(); ++i) accum_buffer[i] += output_buffer[i];
        n_frames++;
        for (int i = 0; i < accum_buffer.size(); ++i) result_buffer[i] = accum_buffer[i] / float(n_frames);

        cv::Mat image(height, width, CV_32FC3, result_buffer.data());
        cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
//        if (!pinhole_camera)
//            cv::flip(image, image, -1);
        cv::imshow(scene.output_file_name(), image);
        key = cv::waitKey(10);
        cv::imwrite(scene.output_file_name() + "random.png", image * 255);
        // std::cin.ignore();

        // using namespace std::chrono_literals;
        // std::this_thread::sleep_for(1s);
    }

    return 0;
}
