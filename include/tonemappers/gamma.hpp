//
// Created by goksu on 5/21/20.
//

#pragma once

#include <opencv2/photo.hpp>

namespace soleil::tonemappers
{
class gamma
{
    cv::Ptr<cv::Tonemap> m_tonemapper;
public:
    gamma(float g = 1.0f);

    [[nodiscard]] cv::Mat process(const cv::Mat& exrimage);
};
}