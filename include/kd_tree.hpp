#pragma once

#include "aabb.hpp"

#include <memory>
#include <optional>

namespace soleil
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
    kd_tree(const std::vector<soleil::primitives::face*>& faces);

    std::optional<soleil::payload> hit(const soleil::ray& ray) const;

    const aabb& bounding_box() const
    {
        return box;
    }

    soleil::primitives::face* object;
};
} // namespace soleil
