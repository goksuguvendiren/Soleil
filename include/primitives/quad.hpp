//
// Created by goksu on 5/20/20.
//

#pragma once

#include "mesh.hpp"

namespace soleil::primitives
{
class quad
{
    soleil::vertex m_a;
    soleil::vertex m_b;
    soleil::vertex m_c;
    soleil::vertex m_d;

    glm::vec3 m_surface_normal;

    std::vector<int> m_material_idx;
public:
    quad(const std::vector<soleil::vertex>& vertices) :  m_a(vertices[0]), m_b(vertices[1]),
                                                         m_c(vertices[2]), m_d(vertices[3])
    {
        m_surface_normal = glm::normalize(glm::cross(m_b.position() - m_a.position(), m_d.position() - m_a.position()));

        m_a.normal = m_surface_normal;
        m_a.m_uv = {0.f, 1.f};

        m_b.normal = m_surface_normal;
        m_b.m_uv = {1.f, 1.f};

        m_c.normal = m_surface_normal;
        m_c.m_uv = {1.f, 0.f};

        m_d.normal = m_surface_normal;
        m_d.m_uv = {0.f, 0.f};
    }

    void transform(const glm::mat4x4& transformer)
    {
        m_a.transform(transformer);
        m_b.transform(transformer);
        m_c.transform(transformer);
        m_d.transform(transformer);
    }

    [[nodiscard]] const soleil::vertex& a() const { return m_a; }
    [[nodiscard]] const soleil::vertex& b() const { return m_b; }
    [[nodiscard]] const soleil::vertex& c() const { return m_c; }
    [[nodiscard]] const soleil::vertex& d() const { return m_d; }

    [[nodiscard]] std::optional<soleil::payload> hit(const soleil::ray& ray) const;

    void add_material(int mat_idx) { m_material_idx.push_back(mat_idx); }
};
}