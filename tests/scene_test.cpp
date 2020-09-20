#include "../doctest/doctest.h"
#include <scene.hpp>
#include <scene_loaders.hpp>

TEST_CASE("Testing Scenes: Quads")
{
    soleil::scene scene = soleil::loaders::load(scene_path);

    auto width = scene.get_camera().width;
    auto height = scene.get_camera().height;

    soleil::renderer<soleil::mc_integrator> r(width, height);

    const auto &camera = scene.get_camera();
    soleil::image_plane plane(camera, width, height);

    auto right = (1 / float(width)) * plane.right;
    auto below = -(1 / float(height)) * plane.up;

    auto pix_center = plane.top_left_position();
    pix_center = pix_center + below * float(j) + right * float(i);
}
