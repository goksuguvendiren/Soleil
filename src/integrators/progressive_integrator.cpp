//
// Created by goksu on 6/7/19.
//

#include "integrators/progressive_integrator.hpp"

#include "camera.hpp"
#include "integrator.hpp"
#include "materials/base.hpp"
#include "scene.hpp"

#include <tonemappers/gamma.hpp>
#include <tonemappers/reinhard.hpp>
#include <vector>

void mouse_click_handler(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == cv::EVENT_LBUTTONDOWN )
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
    }
    else if  ( event == cv::EVENT_RBUTTONDOWN )
    {
        std::cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
    }
}

std::vector<glm::vec3> soleil::progressive_integrator::render(const soleil::scene& scene)
{
    std::vector<glm::vec3> accum_buffer;
    std::vector<glm::vec3> result_buffer;

    auto w = m_mc_integrator.width();
    auto h = m_mc_integrator.height();

    accum_buffer.resize(w * h);
    result_buffer.resize(w * h);

    int key = 0;
    int n_frames = 0;

    cv::namedWindow(scene.output_file_name(), 1);
    cv::setMouseCallback(scene.output_file_name(), mouse_click_handler, nullptr);

    while (key != 27)
    {
        m_mc_integrator.render(scene);
        const auto& frame_buffer = m_mc_integrator.get_frame_buffer();

        for (int i = 0; i < accum_buffer.size(); ++i) accum_buffer[i] += frame_buffer[i];
        n_frames++;
        for (int i = 0; i < accum_buffer.size(); ++i) result_buffer[i] = accum_buffer[i] / float(n_frames);

        cv::Mat image(h, w, CV_32FC3, result_buffer.data());
//        cv::Mat rgb_img;
        cv::cvtColor(image, image, cv::COLOR_BGR2RGB);

//        auto tonemapped = soleil::tonemappers::gamma(2.2).process(rgb_img);

        cv::imshow(scene.output_file_name(), image);

//        cv::imwrite("converted" + scene.output_hdr_name(), rgb_img);
        cv::imwrite(scene.output_hdr_name(), image);
        cv::imwrite(scene.output_file_name(), image * 255);

        key = cv::waitKey(100);

        std::cerr << "Frame count: " << n_frames << '\n';
    }

    return result_buffer;
}

glm::vec3 soleil::progressive_integrator::render_pixel(const soleil::scene& scene, int i, int j)
{
    return m_mc_integrator.render_pixel(scene, i, j);
}
