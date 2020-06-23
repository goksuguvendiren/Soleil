//
// Created by goksu on 6/22/20.
//

#pragma once

#include "bsdf.hpp"

#include <glm/vec3.hpp>
#include <scene.hpp>
#include <string>
#include <textures/sampler2D.hpp>

namespace soleil::bsdfs
{
class lambert
{
public:
    lambert(const glm::vec3& albedo, soleil::textures::sampler2D* texture, std::string name)
        : m_albedo(albedo)
        , m_texture(texture)
        , m_name(std::move(name))
    {}

    glm::vec3 f(const scene& scene, const payload& pld, const glm::vec3& light_direction) const;
    glm::vec3 sample(const glm::vec3& hit_normal, const payload& pld) const;

private:
    glm::vec3 m_albedo;
    std::string m_name;
    soleil::textures::sampler2D* m_texture;
};
} // namespace sole