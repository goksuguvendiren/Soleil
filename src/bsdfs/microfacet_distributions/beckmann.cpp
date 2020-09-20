//
// Created by goksu on 6/24/20.
//

#include <bsdfs/bsdf.hpp>
#include <bsdfs/microfacet_distribution/beckmann.hpp>

namespace soleil::bsdfs::mf_distributions
{
// beckmann microfacet distribution
float beckmann::D(const glm::vec3& w_h) const
{
    auto tan_theta_2 = utils::tan_2_theta(w_h);

    if (std::isinf(tan_theta_2)) return 0.f;

    auto cos_theta = utils::cos_theta(w_h);
    auto cos_phi_2 = utils::cos_2_phi(w_h);
    auto sin_phi_2 = utils::sin_2_phi(w_h);

    auto cos_4_theta = std::pow(cos_theta, 4);

    auto exponent = -tan_theta_2 * (cos_phi_2 / (m_alpha_x * m_alpha_x) + sin_phi_2 / (m_alpha_y * m_alpha_y));

    return std::exp(exponent) / (glm::pi<float>() * m_alpha_x * m_alpha_y * cos_4_theta);
}

float beckmann::D(float cos_theta) const
{
    auto cos_2_theta = cos_theta * cos_theta;
    auto sin_2_theta = std::max(0.f, 1.f - cos_2_theta);

    auto tan_theta_sq = sin_2_theta / cos_2_theta;

    if (std::isinf(tan_theta_sq)) return 0.f;

    auto cos_phi_sq = glm::radians(30.f);
    auto sin_phi_sq = glm::radians(30.f);

    auto exponent = -tan_theta_sq * (cos_phi_sq / (m_alpha_x * m_alpha_x) + sin_phi_sq / (m_alpha_y * m_alpha_y));

    return std::exp(exponent) / (glm::pi<float>() * m_alpha_x * m_alpha_y * std::pow(cos_theta, 4));
}

float beckmann::G1(const glm::vec3& w) const
{
    return 1.f / (1.f + lambda(w));
}

float beckmann::G(const glm::vec3& w_o, const glm::vec3& w_i) const
{
    return 1.f / (1.f + lambda(w_o) + lambda(w_i));
}

float beckmann::alpha(const glm::vec3& w) const
{
    auto cos_2_phi = utils::cos_2_phi(w);
    auto sin_2_phi = utils::sin_2_phi(w);
    return std::sqrt(cos_2_phi * m_alpha_x * m_alpha_x + sin_2_phi * m_alpha_y * m_alpha_y);
}

float beckmann::lambda(const glm::vec3& w) const
{
    auto alph = alpha(w);
    auto tan_theta = std::abs(utils::tan_theta(w));
    if (std::isinf(tan_theta))
        return 0.f;

    auto a = 1.f / (alph * tan_theta);
    if (a >= 1.6f)
        return 0.f;

    auto val = (1.f - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
    return (std::max(0.f, val));
}
} // namespace soleil::bsdfs::mf_distributions