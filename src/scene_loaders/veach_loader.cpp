//
// Created by goksu on 4/13/20.
//

#include "scene_loaders.hpp"
#include "scene_io.h"

inline soleil::primitives::face::normal_types to_rtr(scene_io::NormType normal)
{
    using soleil::primitives::face;
    switch (normal)
    {
        case scene_io::NormType::PER_FACE_NORMAL:
            return face::normal_types::per_face;
        case scene_io::NormType::PER_VERTEX_NORMAL:
            return face::normal_types::per_vertex;
    }
}

inline soleil::primitives::face::material_binding to_rtr(scene_io::MaterialBinding material)
{
    using soleil::primitives::face;
    switch (material)
    {
        case scene_io::MaterialBinding::PER_OBJECT_MATERIAL:
            return face::material_binding::per_object;
        case scene_io::MaterialBinding::PER_VERTEX_MATERIAL:
            return face::material_binding::per_vertex;
    }
}

namespace soleil
{
namespace loaders
{
soleil::scene load_from_veach(const std::string& filename)
{
    soleil::scene_information info;

    auto io = scene_io::readScene(filename.c_str());
    auto& cam = io->camera;
    int width = 400;
    int height = 400;
    info.camera = soleil::camera(to_vec3(cam->position), to_vec3(cam->viewDirection), to_vec3(cam->orthoUp),
                                 soleil::radians{cam->verticalFOV}, width, height);
    //    camera = soleil::camera(to_vec3(cam->position), to_vec3(cam->viewDirection), to_vec3(cam->orthoUp),
    //    cam->focalDistance, cam->verticalFOV, 12.f, false);

    auto* light = io->lights;
    auto brightness = 50.f;
    while (light != nullptr)
    {
        if (light->type == scene_io::LightType::POINT_LIGHT)
        {
            info.lights.emplace_back(to_vec3(light->position), to_vec3(light->color) * brightness);
        } else if (light->type == scene_io::LightType::DIRECTIONAL_LIGHT)
        {
            info.dir_lights.emplace_back(to_vec3(light->direction), to_vec3(light->color));
        }
        light = light->next;
    }

    int id = 0;

    std::vector<std::unique_ptr<soleil::materials::base>> all_materials;

    auto* obj = io->objects;
    while (obj != nullptr) // iterate through the objects
    {
        if (obj->type == scene_io::ObjType::SPHERE_OBJ)
        {
            auto data = reinterpret_cast<scene_io::SphereIO*>(obj->data);

            info.spheres.emplace_back(obj->name ? obj->name : "", to_vec3(data->origin), data->radius,
                                      to_vec3(data->xaxis), data->xlength, to_vec3(data->yaxis), data->ylength,
                                      to_vec3(data->zaxis), data->zlength);

            auto& sph = info.spheres.back();

            //            std::cerr << glm::length(sph.origin - to_vec3(cam->position)) << '\n';
            sph.id = id++;
            for (int i = 0; i < obj->numMaterials; ++i)
            {
                all_materials.emplace_back(std::make_unique<soleil::materials::base>(
                    to_vec3(obj->material->diffColor), to_vec3(obj->material->ambColor),
                    to_vec3(obj->material->specColor), to_vec3(obj->material->emissColor), obj->material->shininess,
                    obj->material->ktran));
                sph.material_idx.push_back(all_materials.size() - 1);
            }
        } else
        {
            auto data = reinterpret_cast<scene_io::PolySetIO*>(obj->data);
            assert(data->type == scene_io::PolySetType::POLYSET_TRI_MESH);

            std::vector<int> material_idx;
            for (int i = 0; i < obj->numMaterials; ++i)
            {
                all_materials.emplace_back(std::make_unique<soleil::materials::base>(
                    to_vec3(obj->material[i].diffColor), to_vec3(obj->material[i].ambColor),
                    to_vec3(obj->material[i].specColor), to_vec3(obj->material[i].emissColor),
                    obj->material[i].shininess, obj->material[i].ktran));

                material_idx.push_back(all_materials.size() - 1); // add the last added material to the list of all materials
            }

            std::vector<soleil::primitives::face> faces;
            for (int i = 0; i < data->numPolys; ++i)
            {
                auto polygon = data->poly[i];
                assert(polygon.numVertices == 3);

                std::array<soleil::vertex, 3> vertices{};
                for (int j = 0; j < polygon.numVertices; ++j)
                {
                    auto& poly = polygon.vert[j];
                    vertices.at(j) =
                        soleil::vertex(to_vec3(poly.pos), to_vec3(poly.norm), poly.materialIndex, poly.s, poly.t);
                }
                faces.emplace_back(vertices, to_rtr(data->normType), to_rtr(data->materialBinding));
            }

            info.meshes.emplace_back(faces, obj->name ? obj->name : "");
            auto& mesh = info.meshes.back();
            mesh.id = id++;
            mesh.material_idx = std::move(material_idx);
        }
        obj = obj->next;
    }
    info.materials = std::move(all_materials);

    info.output_file_name = "veach_file.png";
    info.output_hdr_name = "veach_file.exr";

    return soleil::scene(std::move(info));
}
}
}
