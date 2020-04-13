//
// Created by goksu on 2/24/20.
//

#define TINYOBJLOADER_IMPLEMENTATION
#include "scene.hpp"
#include "tinyobjloader.hpp"

#include <iostream>
#include <materials/texture.hpp>

namespace soleil
{
namespace loaders
{
inline std::string GetBaseDir(const std::string& filepath)
{
    if (filepath.find_last_of("/\\") != std::string::npos)
        return filepath.substr(0, filepath.find_last_of("/\\"));
    return "";
}

inline std::string get_file_name(const std::string& fullpath)
{
    int begin_index = 0;
    if (fullpath.find_last_of("/\\") != std::string::npos)
        begin_index = fullpath.find_last_of("/\\");
    auto last_dot = fullpath.find_last_of(".");
    assert(last_dot != std::string::npos);

    return fullpath.substr(begin_index + 1, last_dot - begin_index - 1);
}

soleil::scene load_obj(const std::string& filename)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    auto base_dir = GetBaseDir(filename);
    if (base_dir.empty())
    {
        base_dir = ".";
    }

    auto obj_name = get_file_name(filename);

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(), base_dir.c_str());

    if (!warn.empty())
    {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty())
    {
        std::cerr << err << std::endl;
    }

    soleil::scene_information info;

    // create a default camera located at the origin, looking at the -z direction.
    auto vertical_fov = glm::radians(33.f);
    auto width = 700;
    auto height = 700;

    info.camera = soleil::camera(glm::vec3{0, 0, 10}, glm::vec3{0, 0, -1},
        glm::vec3{0, 1, 0}, soleil::radians{vertical_fov}, width, height, true);

    // create default light sources
    info.lights.emplace_back(glm::vec3{20, 20, 30}, glm::vec3{1000, 1000, 1000});
    info.lights.emplace_back(glm::vec3{ 0, 20, 30}, glm::vec3{1000, 1000, 1000});
    info.lights.emplace_back(glm::vec3{-20, 20, 30}, glm::vec3{1000, 1000, 1000});

    int id = 0;
    for (auto& shape : shapes)
    {
        std::cerr << "Loading mesh : " << id << "\n";
        std::cerr << shape.mesh.indices.size() << '\n';
        std::vector<soleil::primitives::face> faces;

        for (int i = 0; i < shape.mesh.indices.size(); i += 3)
        {
            std::array<soleil::vertex, 3> face_vertices{};
            for (int j = 0; j < 3; ++j)
            {
                tinyobj::index_t idx = shape.mesh.indices[i + j];

                auto vx = attrib.vertices[3 * idx.vertex_index + 0];
                auto vy = attrib.vertices[3 * idx.vertex_index + 1];
                auto vz = attrib.vertices[3 * idx.vertex_index + 2];

                auto nx = attrib.normals[3 * idx.normal_index + 0];
                auto ny = attrib.normals[3 * idx.normal_index + 1];
                auto nz = attrib.normals[3 * idx.normal_index + 2];

                int current_material_id = shape.mesh.material_ids[i]; // no material if -1

                float tu = 0.f;
                float tv = 0.f;

                if (attrib.texcoords.size() > 0 && (idx.texcoord_index >= 0)) // has valid texture coords
                {
                    tu = attrib.texcoords[2 * idx.texcoord_index + 0];
                    tv = attrib.texcoords[2 * idx.texcoord_index + 1];
                }

                face_vertices[j] = soleil::vertex(glm::vec3{vx, vy, vz}, glm::vec3{nx, ny, nz}, glm::vec2{tu, tv});
            }
            soleil::primitives::face face_new(face_vertices, soleil::primitives::face::normal_types::per_vertex,
                                              soleil::primitives::face::material_binding::per_object);
            faces.push_back(face_new);
        }

        std::string tex_name = "spot_texture";
        soleil::materials::texture tex("../Scenes/obj/spot/" + tex_name + ".png");
        info.materials.push_back(std::make_unique<soleil::materials::base>(soleil::materials::base({0.63, 0.065, 0.05}, std::move(tex), tex_name)));

        info.meshes.emplace_back(faces, "");

        auto& m = info.meshes.back();
        m.id = id++;
        m.material_idx.push_back(0);

        auto transformer = glm::translate(glm::identity<glm::mat4x4>(), {-(m.bounding_box().max + m.bounding_box().min) / 2.f});
        m.transform(transformer);

        transformer = glm::scale(glm::identity<glm::mat4x4>(), {2.5f, 2.5f, 2.5f});
        m.transform(transformer);

        transformer = glm::rotate(glm::identity<glm::mat4x4>(), glm::radians(140.f), {0.f, 1.f, 0.f});
        m.transform(transformer);

        info.output_file_name = obj_name + ".png";
        info.output_hdr_name = obj_name + ".exr";
    }

    return soleil::scene(std::move(info));
}
}
}