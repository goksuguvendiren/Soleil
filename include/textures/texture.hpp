//
// Created by goksu on 5/20/20.
//

#pragma once

#include "sampler2D.hpp"

namespace soleil::textures
{
class texture : public sampler2D
{
    int width;
    int height;

    cv::Mat image_data;

public:
    explicit texture(const std::string& image_path)
    {
        image_data = cv::imread(image_path, cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);

        width = image_data.cols;
        height = image_data.rows;

//        cv::imshow("image", image_data);
//        cv::waitKey(0);
    }

    [[nodiscard]] glm::vec3 get_sample(const glm::vec2& uv) const override
    {
        return get_sample(uv.x, uv.y);
    }

    [[nodiscard]] glm::vec3 get_sample(float u, float v) const override
    {
        u = fmod(u, 1.f);
        if (u < 0) u += 1.f;

        v = fmod(v, 1.f);
        if (v < 0) v += 1.f;

        auto u_img = u * width;
        auto v_img = (1 - v) * height;

        if (u_img == width) u_img--;
        if (v_img == height) v_img--;

        auto color = image_data.at<cv::Vec3b>(v_img, u_img);

        return glm::vec3(color[0] / 255.f, color[1] / 255.f, color[2] / 255.f);
    }

};
}