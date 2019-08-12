#include <iostream>
#include <scene_io.h>
#include "renderer.hpp"
#include "scene.hpp"
#include "loaders/scene_loaders.hpp"
#include "photon_integrator.hpp"
#include "mc_integrator.hpp"

void save_image(const std::string& filename, const std::vector<glm::vec3>& buffer, unsigned int width, unsigned int height)
{
    std::fstream file(filename);

    file << "P3\n";
    file << width << " " << height << "\n255\n";

    for (int i = 0; i < buffer.size(); ++i)
    {
        file << buffer[i].r << " " << buffer[i].g << " " << buffer[i].b << '\n';
    }
}

int main(int argc, const char** argv)
{
    auto begin = std::chrono::system_clock::now();

//    std::string scene_path = "../../Scenes/obj/dragon/dragon.obj";
    std::string scene_path = "../../Scenes/xml/cornellbox_ldr.xml";
//    std::string scene_path = "../../Scenes/obj/CornellBox/CornellBox-Original.obj";
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
    
    rtr::scene scene = rtr::loaders::load_random_spheres(); //rt::loaders::load(scene_path);
    scene.camera.pinhole = pinhole_camera;
    scene.camera.image_plane_dist = image_plane_distance;
    scene.camera.lens_width = lens_width;
    scene.camera.focal_dist = focal_length;

    auto end = std::chrono::system_clock::now();
    std::cerr << "Scene loading took : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " millisecs.\n";

    auto width = 400;
    auto height = 400;
    rtr::renderer<rtr::mc_integrator> r(width, height);
    std::vector<glm::vec3> accum_buffer;
    std::vector<glm::vec3> result_buffer;
    std::vector<glm::vec3> output_buffer;
    
    accum_buffer.resize(width * height);
    result_buffer.resize(width * height);
    
    int n_frames = 0;
    
    int key = 0;
    while(key != 27)
    {
        begin = std::chrono::system_clock::now();
        output_buffer = r.render(scene);
        end = std::chrono::system_clock::now();
        
        std::cerr << "Rendering took : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " millisecs.\n";
        
        for(int i = 0; i < output_buffer.size(); ++i)
            accum_buffer[i] += output_buffer[i];
        n_frames++;
        for(int i = 0; i < accum_buffer.size(); ++i)
            result_buffer[i] = accum_buffer[i] / float(n_frames);
        
        cv::Mat image(width, height, CV_32FC3, result_buffer.data());
        cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
        if (!pinhole_camera)
            cv::flip(image, image, -1);
        cv::imshow("window", image);
        key = cv::waitKey(0);
    }
    save_image("output.ppm", result_buffer, width, height);

    return 0;
}
