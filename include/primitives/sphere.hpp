//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include "ray.hpp"

#include <functional>
#include <glm/vec3.hpp>
#include <materials/base.hpp>
#include <optional>
#include <string>

namespace rtr
{
struct payload;
namespace primitives
{
class sphere
{
public:
    sphere(const std::string& nm, const glm::vec3& pos, float r, const glm::vec3& x_ax, float x_l,
           const glm::vec3& y_ax, float y_l, const glm::vec3& z_ax, float z_l)
        : name(nm)
        , origin(pos)
        , radius(r)
        , x_axis(x_ax)
        , x_len(x_l)
        , y_axis(y_ax)
        , y_len(y_l)
        , z_axis(z_ax)
        , z_len(z_l)
    {}

    sphere(const std::string& nm, const glm::vec3& pos, float r, int m_idx)
        : name(nm)
        , origin(pos)
        , radius(r)
    {
        material_idx.push_back(m_idx);
    }

    sphere()
        : name("default sphere")
        , origin(glm::vec3(0, 0, 0))
        , radius(1.f)
    {
        // materials.push_back(rtr::materials::base());
    }

    std::vector<int> material_idx;
    std::optional<rtr::payload> hit(const rtr::ray& ray) const;

    // private:
    std::string name;
    int id;

    glm::vec3 origin;
    float radius;

    glm::vec3 x_axis;
    float x_len;
    glm::vec3 y_axis;
    float y_len;
    glm::vec3 z_axis;
    float z_len;
};
} // namespace primitives
} // namespace rtr
