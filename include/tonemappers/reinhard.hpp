//
// Created by goksu on 5/21/20.
//

#pragma once

#include <opencv2/photo.hpp>

namespace soleil::tonemappers
{
class reinhard
{
    cv::Ptr<cv::TonemapReinhard> m_tonemapper;
public:
    reinhard(float gamma = 1.0f, float intensity = 0.0f, float light_adapt = 1.0f, float color_adapt = 0.0f);

    [[nodiscard]] cv::Mat process(const cv::Mat& exrimage);
};
}