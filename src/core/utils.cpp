//
// Created by Göksu Güvendiren on 9/20/20.
//

#include <utils.hpp>

orthonormal_basis create_onb(const glm::vec3& normal)
{
    // create am orthonormal basis whose up(y) direction is aligned with the normal
    glm::vec3 n_t = {0, 0, 0}; // z coord
    if (std::fabs(normal.x) > std::fabs(normal.y))
        n_t = glm::vec3(normal.z, 0, -normal.x) / std::sqrt(normal.x * normal.x + normal.z * normal.z);
    else
        n_t = glm::vec3(0, -normal.z, normal.y) / std::sqrt(normal.y * normal.y + normal.z * normal.z);

    auto n_b = glm::cross(normal, n_t); // x coord

    return orthonormal_basis{n_b, normal, n_t};
}