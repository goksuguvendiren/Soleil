//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include "textures/sampler2D.hpp"
#include "utils.hpp"

#include <glm/vec3.hpp>
#include <utility>
#include <vector>

namespace soleil
{
class scene;
class payload;

enum class PathType
{
    Absorbed,
    Transmitted,
    Reflected
};

namespace materials
{
class base
{
public:
    base()
        : diffuse({0.0f, 0.0f, 0.0f})
        , ambient({0.0f, 0.0f, 0.0f})
        , specular({0.f, 0.f, 0.f})
        , emissive(0)
        , exp(0)
        , trans(0)
        , refr_index(1.f)
        , m_texture(nullptr)
    {}

    base(const glm::vec3& diff)
        : diffuse(diff)
        , ambient({0.0f, 0.0f, 0.0f})
        , specular({0.f, 0.f, 0.f})
        , emissive(0)
        , exp(0)
        , trans(0)
        , refr_index(1.f)
        , m_texture(nullptr)
    {}

    base(const glm::vec3& diff, soleil::textures::sampler2D* texture, std::string name)
        : diffuse(diff)
        , ambient({0.0f, 0.0f, 0.0f})
        , specular({0.f, 0.f, 0.f})
        , emissive(0)
        , exp(0)
        , trans(0)
        , refr_index(1.f)
        , m_texture(texture)
        , m_name(name)
    {}

    base(const glm::vec3& diff, int texture_idx)
        : diffuse(diff)
        , ambient({0.0f, 0.0f, 0.0f})
        , specular({0.f, 0.f, 0.f})
        , emissive(0)
        , exp(0)
        , trans(0)
        , refr_index(1.f)
        , m_texture(nullptr)
    {}

    base(const glm::vec3& diff, const glm::vec3& amb, const glm::vec3& spec, const glm::vec3& ems, float p, float t)
        : diffuse(diff)
        , ambient(amb)
        , specular(spec)
        , emissive(ems)
        , exp(p)
        , trans(t)
        , refr_index(1.0f)
        , m_texture(nullptr)
    {
        if (trans > 0.f)
            refr_index = 1.5f;
    }

    virtual glm::vec3 f(const scene& scene, const payload& pld, const glm::vec3& light_direction) const;

    soleil::PathType russian_roulette() const
    {
        auto rr = get_random_float(0, 1);
        if (rr < 0.1)
            return PathType::Absorbed;
        else if (rr < specular.x)
            return PathType::Reflected;
        else if (rr < trans)
            return PathType::Transmitted;
        return PathType::Absorbed;
    }

    virtual glm::vec3 sample(const glm::vec3& hit_normal, const payload& pld) const;

    virtual bool is_emissive() const
    {
        return false;
    }

    virtual bool is_mirror() const { return false; }

    glm::vec3 diffuse;
    glm::vec3 ambient;
    glm::vec3 specular;
    glm::vec3 emissive;

    float exp;
    float trans;
    float refr_index;

    std::string m_name;

private:
    soleil::textures::sampler2D* m_texture;
};
} // namespace materials
} // namespace soleil
