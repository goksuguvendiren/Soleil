//
// Created by goksu on 5/27/20.
//

#pragma once

#include "base.hpp"

#include <bsdfs/cooktorrance.hpp>
#include <utility>
namespace soleil::materials
{
class plastic : public base
{
public:
    plastic(const glm::vec3& kd, const glm::vec3& ks, bsdfs::mf_distributions::beckmann distribution, bsdfs::fresnels::dielectric fresnel, std::string name) :
    m_kd(kd), m_ks(ks), m_name(std::move(name)), m_brdf(kd, std::move(distribution), std::move(fresnel)), m_texture(nullptr)
    {}

    plastic(soleil::textures::sampler2D* texture, const glm::vec3& ks, bsdfs::mf_distributions::beckmann distribution, bsdfs::fresnels::dielectric fresnel, std::string name) :
        m_kd(0.f), m_ks(ks), m_name(std::move(name)), m_texture(texture), m_brdf(texture, std::move(distribution), std::move(fresnel))
    {}

    glm::vec3 f(const scene& scene, const payload& pld, const glm::vec3& light_direction) const override;

    glm::vec3 sample(const glm::vec3 &hit_normal, const payload &pld) const override;

private:
    glm::vec3 m_kd;
    glm::vec3 m_ks;
    std::string m_name;
    soleil::textures::sampler2D* m_texture;

    bsdfs::cooktorrance m_brdf;
};
}