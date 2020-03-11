//
// Created by goksu on 6/7/19.
//

#include "integrators/progressive_integrator.hpp"

#include "camera.hpp"
#include "integrator.hpp"
#include "materials/base.hpp"
#include "scene.hpp"

#include <renderer.hpp>
#include <thread>
#include <vector>

std::vector<glm::vec3> rtr::progressive_integrator::render(const rtr::scene& scene)
{
    std::vector<glm::vec3> accum_buffer;
    std::vector<glm::vec3> result_buffer;

    auto w = m_mc_integrator.width();
    auto h = m_mc_integrator.height();

    accum_buffer.resize(w * h);
    result_buffer.resize(w * h);

    int key = 0;
    int n_frames = 0;

    while (key != 27)
    {
        m_mc_integrator.render(scene);
        const auto& frame_buffer = m_mc_integrator.get_frame_buffer();

        for (int i = 0; i < accum_buffer.size(); ++i) accum_buffer[i] += frame_buffer[i];
        n_frames++;
        for (int i = 0; i < accum_buffer.size(); ++i) result_buffer[i] = accum_buffer[i] / float(n_frames);

        cv::Mat image(h, w, CV_32FC3, result_buffer.data());
        cv::Mat rgb_img;
        cv::cvtColor(image, rgb_img, cv::COLOR_BGR2RGB);
        cv::imshow(scene.output_file_name(), rgb_img);

        cv::imwrite(scene.output_hdr_name(), rgb_img);
        cv::imwrite(scene.output_file_name(), rgb_img * 255);

        key = cv::waitKey(100);

        std::cerr << "Frame count: " << n_frames << '\n';
    }

    return result_buffer;
}
glm::vec3 rtr::progressive_integrator::render_pixel(const rtr::scene& scene, int i, int j)
{
    return m_mc_integrator.render_pixel(scene, i, j);
}
