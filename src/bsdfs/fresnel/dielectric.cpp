//
// Created by goksu on 6/24/20.
//
#include <algorithm>
#include <bsdfs/fresnel/dielectric.hpp>
#include <cassert>
#include <cmath>

namespace soleil::bsdfs::fresnels
{
float snell(float eta_i, float eta_t, float cos_theta_i)
{
    auto sin_theta_i = std::sqrt(std::max(0.f, 1.f - cos_theta_i * cos_theta_i));
    auto sin_theta_t = (eta_i / eta_t) * sin_theta_i;

    auto cos_theta_t = std::sqrt(std::max(0.f, 1.f - sin_theta_t * sin_theta_t));

    return cos_theta_t;
}

float dielectric::F(float cos_theta_i) const
{
    assert(cos_theta_i == std::clamp(cos_theta_i, -1.f, 1.f));

    auto eta_i = m_eta_i;
    auto eta_t = m_eta_t;

    if (cos_theta_i <= 0.f) // means entering, swap indices
    {
        std::swap(eta_i, eta_t);
        cos_theta_i = std::abs(cos_theta_i);
    }

    auto cos_theta_t = snell(eta_i, eta_t, cos_theta_i);

    auto parallel = (eta_t * cos_theta_i - eta_i * cos_theta_t) / (eta_t * cos_theta_i + eta_i * cos_theta_t);
    auto perpendicular = (eta_i * cos_theta_i - eta_t * cos_theta_t) / (eta_i * cos_theta_i + eta_t * cos_theta_t);

    auto reflectance = 0.5f * (parallel * parallel + perpendicular * perpendicular);
    return reflectance;
}
} // namespace soleil::bsdfs::fresnels