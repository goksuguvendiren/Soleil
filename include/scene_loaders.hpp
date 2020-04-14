#pragma once

#include <iostream>
#include <map>

#include "scene.hpp"
#include "tinyxml/tinyxml2.h"

inline soleil::primitives::face::normal_types to_rtr(NormType normal)
{
    using soleil::primitives::face;
    switch (normal)
    {
    case NormType::PER_FACE_NORMAL:
        return face::normal_types::per_face;
    case NormType::PER_VERTEX_NORMAL:
        return face::normal_types::per_vertex;
    }
}

inline soleil::primitives::face::material_binding to_rtr(MaterialBinding material)
{
    using soleil::primitives::face;
    switch (material)
    {
    case MaterialBinding::PER_OBJECT_MATERIAL:
        return face::material_binding::per_object;
    case MaterialBinding::PER_VERTEX_MATERIAL:
        return face::material_binding::per_vertex;
    }
}

inline soleil::vertex get_vertex(std::istringstream& stream, int id)
{
    soleil::vertex vert;

    float datax;
    float datay;
    float dataz;

    stream >> datax;
    stream >> datay;
    stream >> dataz;

    return soleil::vertex{glm::vec3{datax, datay, dataz}};
}

inline std::vector<soleil::vertex> LoadVertexData(tinyxml2::XMLElement *elem)
{
    std::istringstream stream{elem->GetText()};

    std::vector<soleil::vertex> verts;
    int id = 0;
    while (stream)
    {
        verts.push_back(get_vertex(stream, id++));
        assert(id == verts.size());
    }

    return verts;
}

inline std::optional<soleil::primitives::face>
get_face(std::istringstream& stream, const std::vector<soleil::vertex>& vertices, int vertexOffset, int texCoordsOffset)
{
    int x, y, z;
    if (!(stream >> x))
        return std::nullopt;
    if (!(stream >> y))
        return std::nullopt;
    if (!(stream >> z))
        return std::nullopt;

    x += vertexOffset;
    y += vertexOffset;
    z += vertexOffset;

    auto pos_a = vertices[x - 1];
    auto pos_b = vertices[y - 1];
    auto pos_c = vertices[z - 1];

    std::array<soleil::vertex, 3> face_vertices;
    face_vertices[0] = pos_a;
    face_vertices[1] = pos_b;
    face_vertices[2] = pos_c;

    return soleil::primitives::face{face_vertices, soleil::primitives::face::normal_types::per_face,
                                 soleil::primitives::face::material_binding::per_object};
}

inline auto GetTransformations(std::istringstream& stream)
{
    std::vector<std::string> result;

    while (stream.good())
    {
        std::string tr;
        stream >> tr;
        result.push_back(tr);
    }

    return result;
}

inline std::vector<soleil::primitives::sphere> load_spheres(tinyxml2::XMLElement *elem, const std::vector<soleil::vertex>& vertices)
{
    std::vector<soleil::primitives::sphere> spheres;

    for (auto child = elem->FirstChildElement("Sphere"); child != NULL; child = child->NextSiblingElement("Sphere"))
    {
        int id;
        child->QueryIntAttribute("id", &id);
        int matID = child->FirstChildElement("Material")->IntText(0);
        int centerID = child->FirstChildElement("Center")->IntText(0);
        float radius = child->FirstChildElement("Radius")->FloatText(0);

        glm::vec3 center = vertices[centerID - 1].position();

        soleil::primitives::sphere sp("", center, radius, matID);
        
        spheres.push_back(std::move(sp));
    }

    return spheres;
}

inline std::vector<soleil::primitives::mesh> load_meshes(tinyxml2::XMLElement *elem, const std::vector<soleil::vertex>& vertices)
{
    std::vector<soleil::primitives::mesh> meshes;

    for (auto child = elem->FirstChildElement("Mesh"); child != nullptr; child = child->NextSiblingElement("Mesh"))
    {
        int id;
        child->QueryIntAttribute("id", &id);

        bool is_art = false;
        if (child->QueryBoolText(&is_art))
        {
            is_art = true;
        }

        auto FaceData = child->FirstChildElement("Faces");
        std::istringstream stream{FaceData->GetText()};
        int vertexOffset = 0;
        int texCoordOffset = 0;

        if (FaceData->QueryIntAttribute("vertexOffset", &vertexOffset))
            ;
        if (FaceData->QueryIntAttribute("textureOffset", &texCoordOffset))
            ;

        std::optional<soleil::primitives::face> fc;
        int index = 1;

        int matID = child->FirstChildElement("Material")->IntText(0);

        std::vector<soleil::primitives::face> faces;
        while ((fc = get_face(stream, vertices, vertexOffset, texCoordOffset)))
        {
            auto tri = *fc;
            faces.push_back(std::move(tri));
        }

        meshes.emplace_back(faces, "");
        auto& mesh = meshes.back();
        mesh.material_idx.push_back(matID);
//        mesh.configure_materials();
    }

    return meshes;
}

namespace soleil
{
namespace loaders
{
inline std::pair<int, int> GetIntPair(tinyxml2::XMLElement* element)
{
    std::pair<int, int> resolution;

    std::istringstream ss {element->GetText()};
    ss >> resolution.first;
    ss >> resolution.second;

    return resolution;
}

inline glm::vec3 GetElem(tinyxml2::XMLElement* element)
{
    glm::vec3 color;

    std::istringstream ss{element->GetText()};
    ss >> color.r;
    ss >> color.g;
    ss >> color.b;

    return color;
}

inline std::unique_ptr<soleil::materials::base> load_material(tinyxml2::XMLElement* child)
{
    int id;
    child->QueryIntAttribute("id", &id);

    auto ambient = GetElem(child->FirstChildElement("AmbientReflectance"));
    auto diffuse = GetElem(child->FirstChildElement("DiffuseReflectance"));
    auto specular = GetElem(child->FirstChildElement("SpecularReflectance"));

    glm::vec3 mirror = {0, 0, 0}, transparency = {0, 0, 0}, glass = {0, 0, 0};
    float phongEx = 0;

    tinyxml2::XMLElement* tmp;
    if ((tmp = child->FirstChildElement("PhongExponent")))
        phongEx = tmp->FloatText(1);

    soleil::materials::base mat{diffuse, ambient, specular, glm::vec3{0, 0, 0}, phongEx, 0};

    if ((tmp = child->FirstChildElement("Transparency"))) // then the material should be a glass (transparent)
    {
        tinyxml2::XMLElement* tmp_m;
        auto transp = GetElem(tmp);
        mat.trans = transp[0];
        float refraction_index = 1.f;
        if ((tmp_m = child->FirstChildElement("RefractionIndex")))
        {
            refraction_index = tmp_m->FloatText(1.f);
        }

        mat.trans = 1.f;
        mat.refr_index = refraction_index;
    }

    return std::make_unique<soleil::materials::base>(mat);
}

inline soleil::light::point load_point_light(tinyxml2::XMLElement* child)
{
    assert(child->Name() == std::string("PointLight"));
    int id;
    child->QueryIntAttribute("id", &id);

    auto position = GetElem(child->FirstChildElement("Position"));
    auto intensity = GetElem(child->FirstChildElement("Intensity"));

    return soleil::light::point(position, intensity);
}

inline soleil::light::directional load_directional_light(tinyxml2::XMLElement* child)
{
    int id;
    child->QueryIntAttribute("id", &id);

    auto direction = GetElem(child->FirstChildElement("Direction"));
    auto radiance = GetElem(child->FirstChildElement("Radiance"));

    return soleil::light::directional(direction, radiance);
}

inline soleil::scene load_from_xml(const std::string& filename)
{
    soleil::scene_information info;

    tinyxml2::XMLDocument document;
    document.LoadFile(filename.c_str());

    if (document.Error())
    {
        document.PrintError();
        std::abort();
    }

    auto docscene = document.FirstChildElement("Scene");

    if (!docscene)
    {
        std::cerr << "Not a valid scene file" << '\n';
        std::abort();
    }

    if (auto elem = docscene->FirstChildElement("BackgroundColor"))
    {
        auto color = GetElem(elem);
        info.background_color = color;
    }

    if (auto elem = docscene->FirstChildElement("ShadowRayEpsilon"))
    {
        info.shadow_ray_epsilon = elem->FloatText();
    }

    if (auto elem = docscene->FirstChildElement("MaxRecursionDepth"))
    {
        info.max_recursion_depth = elem->IntText(1);
    }

    if (auto elem = docscene->FirstChildElement("IntersectionTestEpsilon"))
    {
        info.intersection_test_epsilon = elem->FloatText();
    }

    if (auto elem = docscene->FirstChildElement("Camera"))
    {
        auto position = GetElem(elem->FirstChildElement("Position"));
        auto view = GetElem(elem->FirstChildElement("Gaze"));
        auto up = GetElem(elem->FirstChildElement("Up"));

        int focal_distance = 12;
        tinyxml2::XMLElement* foc;
        if ((foc = elem->FirstChildElement("FocusDistance")))
        {
            focal_distance = foc->IntText(1);
        }

        std::cerr << "fov may be wrong?\n";
        auto vertical_fov = 0.98f;

        auto res = GetIntPair(elem->FirstChildElement("ImageResolution"));

        info.camera = soleil::camera(position, view, up, soleil::radians{vertical_fov}, 400, 400);
    }
    else
    {
        std::cerr << "Could not read camera information\n";
        std::abort();
    }

    if (auto elem = docscene->FirstChildElement("Lights"))
    {
        for (auto child = elem->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
        {
            if (child->Name() == std::string("PointLight"))
                info.lights.push_back(load_point_light(child));
            else
                info.dir_lights.push_back(load_directional_light(child));
        }
    }

    std::vector<std::unique_ptr<soleil::materials::base>> materials;
    if (auto elem = docscene->FirstChildElement("Materials"))
    {
        for (auto child = elem->FirstChildElement("Material"); child != NULL; child = child->NextSiblingElement())
        {
            materials.push_back(load_material(child));
        }
    }
    info.materials = std::move(materials);

    std::vector<soleil::vertex> vertices;
    if (auto elem = docscene->FirstChildElement("VertexData"))
    {
        vertices = LoadVertexData(elem);
    }
    //
    //        if (auto elem = docscene->FirstChildElement("Transformations")){
    //            transformations  = LoadTransformations(elem);
    //        }

    //        if (auto elem = docscene->FirstChildElement("Textures"))
    //        {
    //            textures = LoadTextures(elem);
    //        }

    if (auto objects = docscene->FirstChildElement("Objects"))
    {
        info.spheres = load_spheres(objects, vertices);
        info.meshes = load_meshes(objects, vertices);
    }

    // scene.print();

    return soleil::scene(std::move(info));
}

inline soleil::scene load_from_veach(const std::string& filename)
{
    soleil::scene_information info;

    auto io = readScene(filename.c_str());
    auto& cam = io->camera;
    int width = 400;
    int height = 400;
    info.camera = soleil::camera(to_vec3(cam->position), to_vec3(cam->viewDirection), to_vec3(cam->orthoUp),
                              soleil::radians{cam->verticalFOV}, width, height);
    //    camera = soleil::camera(to_vec3(cam->position), to_vec3(cam->viewDirection), to_vec3(cam->orthoUp),
    //    cam->focalDistance, cam->verticalFOV, 12.f, false);

    auto* light = io->lights;
    while (light != nullptr)
    {
        if (light->type == LightType::POINT_LIGHT)
        {
            info.lights.emplace_back(to_vec3(light->position), to_vec3(light->color));
        } else if (light->type == LightType::DIRECTIONAL_LIGHT)
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
        if (obj->type == ObjType::SPHERE_OBJ)
        {
            auto data = reinterpret_cast<SphereIO*>(obj->data);

            info.spheres.emplace_back(obj->name ? obj->name : "", to_vec3(data->origin), data->radius,
                                      to_vec3(data->xaxis), data->xlength, to_vec3(data->yaxis), data->ylength,
                                      to_vec3(data->zaxis), data->zlength);

            auto& sph = info.spheres.back();

            //            std::cerr << glm::length(sph.origin - to_vec3(cam->position)) << '\n';
            sph.id = id++;
            for (int i = 0; i < obj->numMaterials; ++i)
            {
                all_materials.emplace_back(std::make_unique<soleil::materials::base>(to_vec3(obj->material->diffColor), to_vec3(obj->material->ambColor),
                                                                                  to_vec3(obj->material->specColor), to_vec3(obj->material->emissColor),
                                                                                  obj->material->shininess, obj->material->ktran));
                sph.material_idx.push_back(all_materials.size() - 1);
            }
        } else
        {
            auto data = reinterpret_cast<PolySetIO*>(obj->data);
            assert(data->type == PolySetType::POLYSET_TRI_MESH);

            std::vector<int> material_idx;
            for (int i = 0; i < obj->numMaterials; ++i)
            {
                all_materials.emplace_back(std::make_unique<soleil::materials::base>(to_vec3(obj->material[i].diffColor), to_vec3(obj->material[i].ambColor),
                                                                                  to_vec3(obj->material[i].specColor), to_vec3(obj->material[i].emissColor),
                                                                                  obj->material[i].shininess, obj->material[i].ktran));
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
                    vertices.at(j) = soleil::vertex(to_vec3(poly.pos), to_vec3(poly.norm),
                                                 poly.materialIndex, poly.s, poly.t);
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
    info.output_hdr_name  = "veach_file.exr";

    return soleil::scene(std::move(info));
}

soleil::scene load_tungsten(const std::string& filename);
soleil::scene load_obj(const std::string& filename);

inline soleil::scene load(const std::string& filename)
{
    if (hasEnding(filename, ".obj"))
    {
        return load_obj(filename);
    }
    else if (hasEnding(filename, ".ascii"))
    {
        return load_from_veach(filename);
    }
    else if (hasEnding(filename, ".xml"))
    {
        return load_from_xml(filename);
    }
    else if (hasEnding(filename, ".json"))
    {
        return load_tungsten(filename);
    }
    else
    {
        throw std::runtime_error("unknown file type!");
    }
}
} // namespace loaders
} // namespace soleil
