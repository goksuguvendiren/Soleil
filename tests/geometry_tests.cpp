#include "../doctest/doctest.h"
#include "primitives/mesh.hpp"
#include "utils.hpp"
#include <glm/glm.hpp>

soleil::primitives::mesh load_quad(const glm::mat4x4 &transform)
{
    std::vector<soleil::vertex> vertices = {soleil::vertex({-0.5f, 0.0f, -0.5f}),
                                         soleil::vertex({0.5f, 0.0f, -0.5f}),
                                         soleil::vertex({0.5f, 0.0f, 0.5f}),
                                         soleil::vertex({-0.5f, 0.0f, 0.5f})};

    for (auto &vertex : vertices)
        vertex.transform(transform);

    std::vector<soleil::primitives::face> faces;
    faces.emplace_back(soleil::primitives::face({vertices[0], vertices[1], vertices[2]},
                                             soleil::primitives::face::normal_types::per_face, soleil::primitives::face::material_binding::per_object));
    faces.emplace_back(soleil::primitives::face({vertices[0], vertices[2], vertices[3]},
                                             soleil::primitives::face::normal_types::per_face, soleil::primitives::face::material_binding::per_object));

    return soleil::primitives::mesh(faces, "quad");
}

TEST_CASE("Testing Quads: Identity transformation")
{
    std::vector<soleil::vertex> vertices = {soleil::vertex({-0.5f, 0.0f, -0.5f}),
                                         soleil::vertex({0.5f, 0.0f, -0.5f}),
                                         soleil::vertex({0.5f, 0.0f, 0.5f}),

                                         soleil::vertex({-0.5f, 0.0f, -0.5f}),
                                         soleil::vertex({0.5f, 0.0f, 0.5f}),
                                         soleil::vertex({-0.5f, 0.0f, 0.5f})};

    auto quad = load_quad(glm::mat4x4(1.f));
    for (int i = 0; i < quad.faces.size(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            CHECK(quad.faces[i].vertices[j].poss == vertices[3 * i + j].poss);
        }
    }
}

TEST_CASE("Testing Quads: Translation")
{
    std::vector<soleil::vertex> vertices = {soleil::vertex({0.5f, -1.0f, 1.5f}),
                                         soleil::vertex({1.5f, -1.0f, 1.5f}),
                                         soleil::vertex({1.5f, -1.0f, 2.5f}),

                                         soleil::vertex({0.5f, -1.0f, 1.5f}),
                                         soleil::vertex({1.5f, -1.0f, 2.5f}),
                                         soleil::vertex({0.5f, -1.0f, 2.5f})};

    auto transform = glm::mat4x4(1.f);
    glm::vec3 translation = {1.0f, -1.0f, 2.0f};
    transform = glm::translate(transform, translation);

    auto quad = load_quad(transform);
    for (int i = 0; i < quad.faces.size(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            CHECK(quad.faces[i].vertices[j].poss == vertices[3 * i + j].poss);
        }
    }
}

// TEST_CASE("Testing Quads: Rotation")
// {
//     std::vector<soleil::vertex> vertices = {soleil::vertex({0.5f, -1.0f, 1.5f}),
//                                          soleil::vertex({1.5f, -1.0f, 1.5f}),
//                                          soleil::vertex({1.5f, -1.0f, 2.5f}),

//                                          soleil::vertex({0.5f, -1.0f, 1.5f}),
//                                          soleil::vertex({1.5f, -1.0f, 2.5f}),
//                                          soleil::vertex({0.5f, -1.0f, 2.5f})};

//     auto transform = glm::mat4x4(1.f);
//     glm::vec3 translation = {1.0f, -1.0f, 2.0f};
//     transform = glm::translate(transform, translation);

//     auto quad = load_quad(transform);
//     for (int i = 0; i < quad.faces.size(); ++i)
//     {
//         for (int j = 0; j < 3; ++j)
//         {
//             CHECK(quad.faces[i].vertices[j].poss == vertices[3 * i + j].poss);
//         }
//     }
// }

// TEST_CASE("Testing Quads: Scale")
// {
//     std::vector<soleil::vertex> vertices = {soleil::vertex({0.5f, -1.0f, 1.5f}),
//                                          soleil::vertex({1.5f, -1.0f, 1.5f}),
//                                          soleil::vertex({1.5f, -1.0f, 2.5f}),

//                                          soleil::vertex({0.5f, -1.0f, 1.5f}),
//                                          soleil::vertex({1.5f, -1.0f, 2.5f}),
//                                          soleil::vertex({0.5f, -1.0f, 2.5f})};

//     auto transform = glm::mat4x4(1.f);
//     glm::vec3 translation = {1.0f, -1.0f, 2.0f};
//     transform = glm::translate(transform, translation);

//     auto quad = load_quad(transform);
//     for (int i = 0; i < quad.faces.size(); ++i)
//     {
//         for (int j = 0; j < 3; ++j)
//         {
//             CHECK(quad.faces[i].vertices[j].poss == vertices[3 * i + j].poss);
//         }
//     }
// }

// TEST_CASE("Testing Quads: Arbitrary transform")
// {
//     std::vector<soleil::vertex> vertices = {soleil::vertex({0.5f, -1.0f, 1.5f}),
//                                          soleil::vertex({1.5f, -1.0f, 1.5f}),
//                                          soleil::vertex({1.5f, -1.0f, 2.5f}),

//                                          soleil::vertex({0.5f, -1.0f, 1.5f}),
//                                          soleil::vertex({1.5f, -1.0f, 2.5f}),
//                                          soleil::vertex({0.5f, -1.0f, 2.5f})};

//     auto transform = glm::mat4x4(1.f);
//     glm::vec3 translation = {1.0f, -1.0f, 2.0f};
//     transform = glm::translate(transform, translation);

//     auto quad = load_quad(transform);
//     for (int i = 0; i < quad.faces.size(); ++i)
//     {
//         for (int j = 0; j < 3; ++j)
//         {
//             CHECK(quad.faces[i].vertices[j].poss == vertices[3 * i + j].poss);
//         }
//     }
// }