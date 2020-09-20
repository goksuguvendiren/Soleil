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
    explicit texture(const std::string& image_path);
    [[nodiscard]] glm::vec3 get_sample(const glm::vec2& uv) const override;
    [[nodiscard]] glm::vec3 get_sample(float u, float v) const override;
};
}