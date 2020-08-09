//
// Created by goksu on 6/18/20.
//

#include "bsdfs/orennayar.hpp"
#include "include/camera.hpp"
int main()
{
    soleil::bsdfs::orennayar orennayar({1.f, 1.f, 1.f}, soleil::radians{40.f}, "debug_orennayar");

    soleil::scene scene;
    soleil::payload pld;
    glm::vec3 direction;
    soleil::ray ray({-10, 10, 1}, {0, 1, 0}, false);

    pld.hit_normal = glm::vec3{1.0, 0, 0};
    pld.ray = ray;

    orennayar.f(scene, pld, direction);

    return 0;
}