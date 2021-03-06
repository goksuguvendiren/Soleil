//
// Created by Göksu Güvendiren on 2019-05-10.
//

#include "primitives/sphere.hpp"

#include "payload.hpp"

#include <iostream>

constexpr double pi = 3.14159265359;

static glm::vec2 get_lat_long(const glm::vec3& point, const glm::vec3& center)
{
    auto N = glm::normalize(point - center);
    auto u = std::atan2(N.x, N.z) / (2 * pi) + 0.5;
    auto v = N.y * 0.5 + 0.5;

    return {u, v};
}

std::optional<soleil::payload> soleil::primitives::sphere::hit(const soleil::ray& ray) const
{
    auto eminc = ray.origin() - origin;

    auto A = 1;
    auto B = 2.0f * glm::dot(ray.direction(), eminc);
    auto C = glm::dot(eminc, eminc) - radius * radius;

    auto delta = B * B - 4 * A * C;

    if (delta < 0)
        return std::nullopt;

    auto param = (-B - std::sqrt(delta));

    if (param < 0)
    {
        param = (-B + std::sqrt(delta));

        if (param < 0)
            return std::nullopt;
    }

    param *= 0.5f;

    auto hit_point = ray.origin() + param * ray.direction();
    auto surface_normal = glm::normalize(hit_point - origin);

    if (std::isinf(param) || std::isnan(param))
    {
        return std::nullopt;
    }

    auto uv = get_lat_long(hit_point, origin);

    assert(material_idx.size() > 0 && " sphere has no materials, hitting the default sphere probably.");

    auto pld = soleil::payload{surface_normal, hit_point, ray, param, material_idx.front(), uv, id};

    return pld;
}
