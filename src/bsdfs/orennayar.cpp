//
// Created by goksu on 6/18/20.
//

#include <bsdfs/orennayar.hpp>
#include <utility>
#include <utils.hpp>

namespace soleil::bsdfs
{
orennayar::orennayar(const glm::vec3& albedo, soleil::radians sigma, std::string  name)
    : m_albedo(albedo)
    , m_sigma(sigma)
    , m_name(std::move(name))
{
    m_sigma_squared = sigma.radian * sigma.radian;

    m_A = 1.f - (m_sigma_squared / (2.f * (m_sigma_squared + 0.33f)));
    m_B = (0.45f * m_sigma_squared) / (m_sigma_squared + 0.09f);
}

// global->local
glm::vec3 apply(const orthonormal_basis& onb, const glm::vec3& vector)
{
    return glm::normalize(glm::vec3(glm::dot(onb.x, vector), glm::dot(onb.y, vector), glm::dot(onb.z, vector)));
}

glm::vec3 orennayar::f(const soleil::scene& scene, const soleil::payload& pld, const glm::vec3& light_direction) const
{
    auto onb = create_onb(glm::normalize(pld.hit_normal));

    auto w_o = apply(onb, glm::normalize(-pld.ray.direction()));
    auto w_i = apply(onb, glm::normalize(light_direction));

    auto cos_theta_o = w_o.y;
    auto sin_theta_o = std::sqrt(std::max(0.f, 1.f - cos_theta_o * cos_theta_o));
    auto cos_phi_o = w_o.x / sin_theta_o;
    auto sin_phi_o = w_o.z / sin_theta_o;

    auto cos_theta_i = w_i.y;
    auto sin_theta_i = std::sqrt(std::max(0.f, 1.f - cos_theta_i * cos_theta_i));
    auto cos_phi_i = w_i.x / sin_theta_i;
    auto sin_phi_i = w_i.z / sin_theta_i;

    auto diff = cos_phi_i * cos_phi_o + sin_phi_i * sin_phi_o;

    auto theta_i_larger = std::abs(cos_theta_i) < std::abs(cos_theta_o);

    float sin_alpha = theta_i_larger ? sin_theta_i : sin_theta_o;
    float tan_beta = theta_i_larger ? (sin_theta_o / std::abs(cos_theta_o)) : (sin_theta_i / std::abs(cos_theta_i));

    auto val = m_A + m_B * std::max(0.f, diff) * sin_alpha * tan_beta;

    return (m_albedo * val) / glm::pi<float>();
}

 glm::vec3 orennayar::sample(const glm::vec3& hit_normal, const payload& pld) const
{
    return sample_hemisphere(hit_normal);
}
} // namespace soleil::materials