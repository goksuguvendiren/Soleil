//
// Created by goksu on 5/21/20.
//

#include <tonemappers/gamma.hpp>
#include <tonemappers/reinhard.hpp>

namespace soleil::tonemappers
{
reinhard::reinhard(float gamma, float intensity, float light_adapt, float color_adapt)
{
    m_tonemapper = cv::createTonemapReinhard(gamma, intensity, light_adapt, color_adapt);
}

cv::Mat reinhard::process(const cv::Mat& exrimage)
{
    cv::Mat outputimage;
    m_tonemapper->process(exrimage, outputimage);

    return outputimage;
}
}