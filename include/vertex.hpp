//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include "materials/base.hpp"

#include <glm/vec3.hpp>

namespace rtr
{
class vertex
{
public:
    vertex() = default;
    vertex(const glm::vec3& pos)
        : poss(pos)
        , normal({0, 0, 0})
        , mat(-1)
        , u(0)
        , v(0)
    {}
    vertex(const glm::vec3& pos, const glm::vec3& n, int m, float s, float t)
        : poss(pos)
        , normal(n)
        , mat(m)
        , u(s)
        , v(t)
    {}

    int mat;
    glm::vec3 normal;

    glm::vec3 poss;
    float u, v;

    glm::vec3 position() const
    {
        return poss;
    }

    void transform(const glm::mat4x4& transform)
    {
        poss = glm::vec3(transform * glm::vec4(poss, 1.0f));
    }
    //        glm::vec3 normal() const { return norm; }
};
} // namespace rtr
