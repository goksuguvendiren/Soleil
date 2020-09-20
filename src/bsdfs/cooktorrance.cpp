//
// Created by goksu on 6/23/20.
//
#include <bsdfs/bsdf.hpp>
#include <bsdfs/cooktorrance.hpp>
#include <bsdfs/orennayar.hpp>
#include <utility>
#include <utils.hpp>

namespace soleil::bsdfs
{
cooktorrance::cooktorrance(const glm::vec3& albedo, mf_distributions::beckmann distribution, fresnels::dielectric fresnel)
    : m_albedo(albedo), m_distribution(std::move(distribution)), m_fresnel(std::move(fresnel)), m_texture(nullptr)
{}

cooktorrance::cooktorrance(soleil::textures::sampler2D* texture, mf_distributions::beckmann distribution,
                           fresnels::dielectric fresnel) : m_albedo(1.0f), m_distribution(std::move(distribution)), m_fresnel(std::move(fresnel)), m_texture(texture)
{}

glm::vec3 cooktorrance::f(const scene& scene, const payload& pld, const glm::vec3& light_direction) const
{
    glm::vec3 albedo = m_albedo;
    if (m_texture)
        albedo = m_texture->get_sample(pld.texture_coords);

    auto onb = create_onb(glm::normalize(pld.hit_normal));

    auto w_o = utils::world_to_local(onb, glm::normalize(-pld.ray.direction()));
    auto w_i = utils::world_to_local(onb, glm::normalize(light_direction));

    auto w_h = glm::normalize(w_o + w_i);

    auto ldoth = glm::dot(w_i, w_h);
    auto ndoth = utils::cos_theta(w_h);

    float n = 10;
    auto res = albedo * (n + 1) * (glm::pow(ndoth, n)) / (8 * glm::pi<float>() * std::pow(ldoth, 3.f));

    if (std::isnan(res.x) || std::isnan(res.y) || std::isnan(res.z) || std::isinf(res.x) || std::isinf(res.y) || std::isinf(res.z))
    {
        std::cerr << "not a number\n";
        return glm::vec3(0.f);
    }

    return res;

    auto cos_theta_o = std::abs(utils::cos_theta(w_o));
    auto cos_theta_i = std::abs(utils::cos_theta(w_i));

    // handle degenerate cases:
    if (cos_theta_i == 0.f || cos_theta_o == 0.f) return glm::vec3(0.f);
    if (w_h.x == 0.f && w_h.y == 0.f && w_h.z == 0.f) return glm::vec3(0.f);

    auto cos_theta_h = glm::dot(w_h, w_i);

    auto distr  = std::visit([&w_h](const auto& distribution) { return distribution.D(w_h); }, m_distribution);
    auto shadow = std::visit([&w_o, &w_i](const auto& distribution) { return distribution.G(w_o, w_i); }, m_distribution);
    auto fresn  = std::visit([&cos_theta_h](auto& fre) { return fre.F(cos_theta_h); }, m_fresnel);

    return (albedo * distr * shadow * fresn) / (4.f * cos_theta_o * cos_theta_i);
}

glm::vec3 cooktorrance::sample(const glm::vec3& hit_normal, const payload& pld) const
{
    return sample_hemisphere(hit_normal);
}
} // namespace soleil::bsdfs