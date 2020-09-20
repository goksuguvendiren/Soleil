//
// Created by goksu on 6/23/20.
//

#pragma once

#include "bsdf.hpp"
#include "bsdfs/microfacet_distribution/beckmann.hpp"
#include "materials/base.hpp"

#include <bsdfs/fresnel/dielectric.hpp>
#include <glm/vec3.hpp>
#include <payload.hpp>
#include <scene.hpp>
#include <string>

namespace soleil::bsdfs
{
class cooktorrance
{
    std::variant<mf_distributions::beckmann> m_distribution;
    std::variant<fresnels::dielectric> m_fresnel;
    glm::vec3 m_albedo;
    soleil::textures::sampler2D* m_texture;

public:
    cooktorrance(const glm::vec3& albedo, mf_distributions::beckmann distribution, fresnels::dielectric fresnel);
    cooktorrance(soleil::textures::sampler2D* texture, mf_distributions::beckmann distribution, fresnels::dielectric fresnel);

    glm::vec3 f(const scene& scene, const payload& pld, const glm::vec3& light_direction) const;

    glm::vec3 sample(const glm::vec3& hit_normal, const payload& pld) const;

private:
};
} // namespace soleil::materials