//
// Created by Göksu Güvendiren on 2019-05-10.
//

#include "utils.hpp"

#include <exception>
#include <iostream>
#include <optional>
#include <payload.hpp>
#include <primitives/mesh.hpp>
#include <ray.hpp>

inline float determinant(const glm::vec3& col1, const glm::vec3& col2, const glm::vec3& col3)
{
    return col1.x * (col2.y * col3.z - col2.z * col3.y) -
           col2.x * (col1.y * col3.z - col1.z * col3.y) +
           col3.x * (col1.y * col2.z - col1.z * col2.y);
}

inline bool is_back_face(const glm::vec3& surface_normal, const glm::vec3& direction)
{
    return glm::dot(surface_normal, direction) < 0;
}

thread_local std::array<soleil::materials::base, 4096> interpolated_material;
thread_local int idx = 0;

//inline soleil::material interpolate_materials(soleil::material* a, float alpha, soleil::material* b, float beta,
//                                           soleil::material* c, float gamma)
//{
//    soleil::materials::base mat;
//
//    auto interpolate = [alpha, beta, gamma](auto propa, auto propb, auto propc)
//    {
//        auto res = propa * alpha + propb * beta + propc * gamma;
//        return res;
//    };
//
//    mat.diffuse    = interpolate(a->diffuse, b->diffuse, c->diffuse);
//    mat.ambient    = interpolate(a->ambient, b->ambient, c->ambient);
//    mat.specular   = interpolate(a->specular, b->specular, c->specular);
//    mat.emissive   = interpolate(a->emissive, b->emissive, c->emissive);
//    mat.exp        = interpolate(a->exp, b->exp, c->exp);
//    mat.trans      = interpolate(a->trans, b->trans, c->trans);
//    mat.refr_index = interpolate(a->refr_index, b->refr_index, c->refr_index);
//
//    return mat;
//}

std::optional<soleil::payload> soleil::primitives::face::hit(const soleil::ray& ray) const
{
    const auto& a = vertices[0];
    const auto& b = vertices[1];
    const auto& c = vertices[2];

    glm::vec3 col1 = a.position() - b.position();
    glm::vec3 col2 = a.position() - c.position();
    glm::vec3 col3 = ray.direction();
    glm::vec3 col4 = a.position() - ray.origin();

    auto epsilon = -1e-7;
    auto detA = determinant(col1, col2, col3);
    if (detA == 0)
        return std::nullopt;

    auto oneOverDetA = 1 / detA;

    auto beta = determinant(col4, col2, col3) * oneOverDetA;
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

//    if (normal_type == normal_types::per_vertex)
//    {
        normal = glm::normalize(alpha * a.normal + beta * b.normal + gamma * c.normal);
        tex_coords = alpha * a.m_uv + beta * b.m_uv + gamma * c.m_uv;
//    }
//    else
//    {
//        normal = glm::normalize(vertices[0].normal);
//        tex_coords = vertices[0].m_uv;
//    }

    if (material_type == material_binding::per_vertex)
    {
        // assert(false && " disabled material_binding::per_vertex, enable this!");
        // auto ind = idx;
        // interpolated_material[ind] = (interpolate_materials(a.mat, alpha, b.mat, beta, c.mat, gamma));
        // idx = (idx + 1) % interpolated_material.size();
        // mtrl_ptr = &interpolated_material[ind];
    }

    if (std::isnan(param))
        throw std::runtime_error("param is nan in face::hit()!");

    return soleil::payload{normal, point, ray, param, -1, tex_coords};
}

void soleil::primitives::face::set_normal()
{
    auto normal = glm::normalize(
        glm::cross(vertices[1].position() - vertices[0].position(), vertices[2].position() - vertices[0].position()));

    for (auto& vert : vertices) vert.normal = normal;
}

std::optional<soleil::payload> soleil::primitives::mesh::hit(const soleil::ray& ray) const
{
    auto hit = tree.hit(ray);

    if (hit)
    {
        if (hit->material_idx == -1)
            hit->material_idx = material_idx.front();
        hit->obj_id = id;
    }

    return hit;
}
