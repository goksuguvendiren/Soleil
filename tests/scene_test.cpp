#include "../doctest/doctest.h"
#include <scene.hpp>
#include <scene_loaders.hpp>

TEST_CASE("Testing Scenes: Quads")
{
    rtr::scene scene = rtr::loaders::load(scene_path);

    auto width = scene.get_camera().width;
    auto height = scene.get_camera().height;

    rtr::renderer<rtr::mc_integrator> r(width, height);

    const auto &camera = scene.get_camera();
    rtr::image_plane plane(camera, width, height);

    auto right = (1 / float(width)) * plane.right;
    auto below = -(1 / float(height)) * plane.up;

    auto pix_center = plane.top_left_position();
    pix_center = pix_center + below * float(j) + right * float(i);
}
