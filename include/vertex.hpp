//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include "materials/base.hpp"

#include <glm/vec3.hpp>

namespace soleil
{
class vertex
{
public:
    vertex() = default;
    vertex(const glm::vec3& pos)
        : poss(pos)
        , normal({0, 0, 0})
        , mat(-1)
        , m_uv(0, 0)
    {}

    vertex(const glm::vec3& pos, const glm::vec3& n)
        : poss(pos)
        , normal(n)
        , mat(-1)
        , m_uv(0, 0)
    {}

    vertex(const glm::vec3& pos, const glm::vec3& n, const glm::vec2 uv)
        : poss(pos)
        , normal(n)
        , mat(-1)
        , m_uv(uv)
    {}

    vertex(const glm::vec3& pos, const glm::vec3& n, int m, float s, float t)
        : poss(pos)
        , normal(n)
        , mat(m)
        , m_uv(s, t)
    {}

    int mat;
    glm::vec3 normal;

    glm::vec3 poss;
    glm::vec2 m_uv;

    glm::vec3& position()
    {
        return poss;
    }

    const glm::vec3& position() const
    {
        return poss;
    }

    void transform(const glm::mat4x4& transform)
    {
        poss = glm::vec3(transform * glm::vec4(poss, 1.0f));

        auto inv_tr_transform = glm::transpose(glm::inverse(transform));
        normal = glm::vec3(inv_tr_transform * glm::vec4(normal, 0.0f));
    }
};
} // namespace soleil
