//
// Created by goksu on 6/23/20.
//
#include "../doctest/doctest.h"

#include <bsdfs/cooktorrance.hpp>
#include <utils.hpp>

bool float_equals(float val1, float val2)
{
    auto epsilon = 1e-5;

    return std::abs(val1 - val2) < epsilon;
}

namespace soleil
{
namespace
{
TEST_CASE("cos theta with orthonormal basis")
{
    using namespace soleil::bsdfs;
    glm::vec3 normal(0.3, 0.3, 0.3);
    normal = glm::normalize(normal);
    glm::vec3 test(0.2, 0.5, 0.8);
    test = glm::normalize(test);

    auto onb = create_onb(normal);
    auto local_test = soleil::bsdfs::utils::world_to_local(onb, test);

    auto cos_theta = soleil::bsdfs::utils::cos_theta(local_test);
    auto cos_theta_true = glm::dot(normal, test);

    // cos theta
    REQUIRE(float_equals(cos_theta, cos_theta_true));

    auto cos_2_true = cos_theta_true * cos_theta_true;
    auto cos_2 = soleil::bsdfs::utils::cos_2_theta(local_test);

    // cos^2 theta
    REQUIRE(float_equals(cos_2, cos_2_true));

    auto theta = glm::acos(cos_theta_true);
    REQUIRE(float_equals(glm::cos(theta), cos_theta_true));

    auto sin_theta_true = glm::sin(theta);
    auto sin_theta = utils::sin_theta(local_test);

    REQUIRE(float_equals(sin_theta_true, sin_theta));

    auto sin_2_true = sin_theta_true * sin_theta_true;
    auto sin_2 = soleil::bsdfs::utils::sin_2_theta(local_test);
    REQUIRE(float_equals(sin_2_true, sin_2));

    auto tan_theta_true = glm::tan(theta);
    auto tan_theta = utils::tan_theta(local_test);
    REQUIRE(float_equals(tan_theta, tan_theta_true));

    auto tan_2_true = tan_theta_true * tan_theta_true;
    auto tan_2 = utils::tan_2_theta(local_test);
    REQUIRE(float_equals(tan_2, tan_2_true));


}
} // namespace
} // namespace soleil