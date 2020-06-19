//
// Created by goksu on 5/21/20.
//

#include <tonemappers/gamma.hpp>

namespace soleil::tonemappers
{
gamma::gamma(float g)
{
    m_tonemapper = cv::createTonemap(g);
}

cv::Mat gamma::process(const cv::Mat& exrimage)
{
    cv::Mat outputimage;
    m_tonemapper->process(exrimage, outputimage);

//    cv::pow(exrimage, 1.0f/2.2f, outputimage);

    return outputimage;
}
}