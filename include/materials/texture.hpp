//
// Created by Göksu Güvendiren on 6.03.2020.
//

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <opencv2/opencv.hpp>
#include <string>

namespace rtr
{
namespace materials
{
class texture
{
    int width;
    int height;

    cv::Mat image_data;

public:
    explicit texture(const std::string& image_path)
    {
        image_data = cv::imread(image_path);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);

        width = image_data.cols;
        height = image_data.rows;
    }

    [[nodiscard]] glm::vec3 getColor(const glm::vec2& uv) const
    {
        return getColor(uv.x, uv.y);
    }

    [[nodiscard]] glm::vec3 getColor(float u, float v) const
    {
        u = fmod(u, 1.f);
        if (u < 0) u += 1.f;

        v = fmod(v, 1.f);
        if (v < 0) v += 1.f;

        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);

        return glm::vec3(color[0] / 255.f, color[1] / 255.f, color[2] / 255.f);
    }
};
}
}