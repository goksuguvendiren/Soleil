#pragma once

#include "aabb.hpp"

#include <memory>
#include <optional>

namespace rtr
{
namespace primitives
{
class mesh;
class face;
} // namespace primitives

class ray;
struct payload;

class kd_tree
{
    std::unique_ptr<kd_tree> left;
    std::unique_ptr<kd_tree> right;

    aabb box;

public:
    kd_tree()
        : left(nullptr)
        , right(nullptr)
    {}
    kd_tree(const std::vector<rtr::primitives::face*>& faces);

    std::optional<rtr::payload> hit(const rtr::ray& ray) const;

    const aabb& bounding_box() const
    {
        return box;
    }

    rtr::primitives::face* object;
};
} // namespace rtr
