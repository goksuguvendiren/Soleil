#pragma once

#include <iostream>
#include <map>

#include "scene.hpp"
#include "tinyxml/tinyxml2.h"

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

soleil::scene load_from_veach(const std::string& filename);
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
