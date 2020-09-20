#include "../doctest/doctest.h"
#include <aabb.hpp>

namespace soleil {
namespace {
TEST_CASE("bounding box works for trivial cases") {
    std::array<vertex, 3> face;
    face[0].position() = {-3, -1, 0};
    face[1].position() = {3, -1, 0};
    face[2].position() = {0, 2, 0};
    aabb box(face);

    REQUIRE_EQ(-3, box.min.x);
    REQUIRE_EQ(3, box.max.x);

    ray r({0, 0, -1}, {0, 0, 1}, 0);

    REQUIRE(box.hit(r));
}
}
}