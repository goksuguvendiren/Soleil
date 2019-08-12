//
// Created by Göksu Güvendiren on 2019-06-17.
//

namespace rtr
{
namespace loaders
{

inline rtr::scene load_random_spheres()
{
    rtr::scene scene;

    int id = 0;
    scene.spheres.emplace_back(std::to_string(id++), glm::vec3{0, -0.05, -3}, 0.5f);
    scene.spheres.emplace_back(std::to_string(id++), glm::vec3{0, -100.5f, -1}, 100.f);

    auto focal_distance = 12.2118f;
    auto vertical_fov = 0.785398f;
    scene.camera = rtr::camera(glm::vec3{0, 0, 0}, glm::vec3{0, 0, -1}, glm::vec3{0, 1, 0}, focal_distance, vertical_fov, focal_distance, false );

    scene.spheres[0].materials.emplace_back(glm::vec3{0.5, 0.5, 0.5}, glm::vec3{0.8, 0.2, 0.2}, glm::vec3{0, 0, 0}, glm::vec3{0, 0, 0}, 0, 0);
    scene.spheres[1].materials.emplace_back(glm::vec3{0.5, 0.5, 0.5}, glm::vec3{0.2, 0.2, 0.2}, glm::vec3{0, 0, 0}, glm::vec3{0, 0, 0}, 0, 0);

    return scene;
}

}
}