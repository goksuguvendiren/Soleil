//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include "aabb.hpp"
#include "kd_tree.hpp"
#include "vertex.hpp"

#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <materials/base.hpp>
#include <optional>
#include <string>
#include <vector>

namespace soleil
{
struct payload;
class ray;
namespace primitives
{
struct face
{
    enum class normal_types
    {
        per_vertex,
        per_face
    };

    enum class material_binding
    {
        per_vertex,
        per_object
    };

    face(std::array<soleil::vertex, 3> vert,
         normal_types norm_type = normal_types::per_vertex,
         material_binding mat_type = material_binding::per_object)
        : vertices(std::move(vert))
        , box(vertices)
        , normal_type(norm_type)
        , material_type(mat_type)
    {
        if (normal_type == normal_types::per_face)
            set_normal();
    }

    void transform(const glm::mat4x4& transformer)
    {
        for (auto& vert : vertices)
        {
            vert.transform(transformer);
        }
        box = aabb(vertices);
    }

    std::array<soleil::vertex, 3> vertices;
    std::optional<soleil::payload> hit(const soleil::ray& ray) const;

    aabb box;

    normal_types normal_type;
    material_binding material_type;

    void set_normal();
};

class mesh
{
public:
    mesh(std::vector<soleil::primitives::face> fcs, const std::string& nm)
        : faces(std::move(fcs))
        , name(nm)
    {
        auto begin = std::chrono::system_clock::now();
        std::vector<soleil::primitives::face*> face_ptrs;
        face_ptrs.reserve(faces.size());
        for (auto& f : faces)
        {
            face_ptrs.push_back(&f);
        }
        tree = soleil::kd_tree(face_ptrs);

        auto end = std::chrono::system_clock::now();
//        std::cout << "BVH construction took : "
//                  << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " millisecs.\n";

    }

    void transform(const glm::mat4x4& transformer)
    {
        for (auto& face : faces)
            face.transform(transformer);

        std::vector<soleil::primitives::face*> face_ptrs;
        face_ptrs.reserve(faces.size());
        for (auto& f : faces)
        {
            face_ptrs.push_back(&f);
        }
        tree = soleil::kd_tree(face_ptrs);
    }

    void update_tree()
    {
        std::vector<soleil::primitives::face*> face_ptrs;
        face_ptrs.reserve(faces.size());
        for (auto& f : faces)
        {
            face_ptrs.push_back(&f);
        }

        tree = soleil::kd_tree(face_ptrs);
    }

    std::vector<int> material_idx;
    [[nodiscard]] std::optional<soleil::payload> hit(const soleil::ray& ray) const;

    int id;
    std::string name;

    mesh(mesh&&) noexcept = default;
    mesh(const mesh&) = delete;

    std::vector<soleil::primitives::face> faces;

    mesh copy() const
    {
        auto fcs = faces;
        auto nm = name;

        auto m = mesh(fcs, nm);
        m.material_idx = material_idx;

        return m;
    }

    void configure_materials()
    {
        for (auto& face : faces)
        {
            for (auto& vertex : face.vertices)
            {
                if (vertex.mat == -1) vertex.mat = material_idx[0];
            }
        }
    }
    [[nodiscard]] const soleil::aabb& bounding_box() const { return tree.bounding_box(); }

private:
    soleil::kd_tree tree;
};
} // namespace primitives
} // namespace soleil