//
// Created by goksu on 5/20/20.
//

#include <optional>
#include <primitives/quad.hpp>
#include <ray.hpp>
#include <payload.hpp>

namespace soleil::primitives
{
inline float determinant(const glm::vec3& col1, const glm::vec3& col2, const glm::vec3& col3)
{
    return col1.x * (col2.y * col3.z - col2.z * col3.y) -
           col2.x * (col1.y * col3.z - col1.z * col3.y) +
           col3.x * (col1.y * col2.z - col1.z * col2.y);
}

std::optional<soleil::payload> hit_face(const soleil::ray& ray, const soleil::vertex& a,  const soleil::vertex& b,  const soleil::vertex& c, int mat_idx)
{
    glm::vec3 col1 = a.position() - b.position();
    glm::vec3 col2 = a.position() - c.position();
    glm::vec3 col3 = ray.direction();
    glm::vec3 col4 = a.position() - ray.origin();

    auto epsilon = -1e-7;
    auto detA = determinant(col1, col2, col3);
    if (detA == 0)
        return std::nullopt;

    auto oneOverDetA = 1 / detA;

    auto beta  = determinant(col4, col2, col3) * oneOverDetA;
    auto gamma = determinant(col1, col4, col3) * oneOverDetA;
    auto param = determinant(col1, col2, col4) * oneOverDetA;
    auto alpha = 1 - beta - gamma;

    if (alpha < epsilon || gamma < epsilon || beta < epsilon || param < epsilon)
    {
        return std::nullopt;
    }

    auto point = ray.origin() + param * ray.direction();
    glm::vec3 normal;
    glm::vec2 tex_coords;

    normal = glm::normalize(alpha * a.normal + beta * b.normal + gamma * c.normal);
    tex_coords = alpha * a.m_uv + beta * b.m_uv + gamma * c.m_uv;

    if (std::isnan(param))
        throw std::runtime_error("param is nan in quad::hit()!");

    return soleil::payload{normal, point, ray, param, mat_idx, tex_coords};

}

std::optional<soleil::payload> quad::hit(const soleil::ray& ray) const
{
    assert(m_material_idx.size() == 1);

    auto first_face = hit_face(ray, m_a, m_b, m_c, m_material_idx[0]);
    if (first_face) return first_face;
    return hit_face(ray, m_a, m_c, m_d, m_material_idx[0]); // second face;
}
}