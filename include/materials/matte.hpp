//
// Created by goksu on 6/22/20.
//

#pragma once

#include "base.hpp"

#include <bsdfs/lambert.hpp>
#include <bsdfs/orennayar.hpp>
#include <utility>

namespace soleil::materials
{
class matte final : public base
{
public:
    matte(const glm::vec3& albedo, soleil::textures::sampler2D* texture, std::string name) : m_bsdf(soleil::bsdfs::lambert(albedo, texture, std::move(name)))
    {}

    matte(const glm::vec3& albedo, soleil::radians sigma, std::string name) : m_bsdf(soleil::bsdfs::orennayar(albedo, sigma, std::move(name)))
    {}

    [[nodiscard]] glm::vec3 f(const scene& scene, const payload& pld, const glm::vec3& light_direction) const override;
    [[nodiscard]] glm::vec3 sample(const glm::vec3& hit_normal, const payload& pld) const override;

    ~matte() = default;

private:
    std::variant<soleil::bsdfs::lambert, soleil::bsdfs::orennayar> m_bsdf;
};
} // namespace soleil::materials