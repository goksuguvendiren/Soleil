#include "materials/emissive.hpp"
#include "nlohmann/json.hpp"
#include "scene_loaders.hpp"
#include "utils.hpp"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

namespace rtr
{
namespace loaders
{
static glm::mat4x4 get_transform_matrix(const nlohmann::json& transform_json)
{
    glm::mat4x4 transform = glm::mat4x4(1.0f);

    if (transform_json.empty())
        return transform;

    auto translate_json = transform_json["position"];

    glm::vec3 translation = to_vec3(translate_json);
    transform = glm::translate(transform, translation);

    auto rotation_json = transform_json["rotation"];
    glm::vec3 rotation = to_vec3(rotation_json);
    transform = glm::rotate(transform, rotation.x, {1, 0, 0});
    transform = glm::rotate(transform, rotation.y, {0, 1, 0});
    transform = glm::rotate(transform, rotation.z, {0, 0, 1});

    auto scale_json = transform_json["scale"];
    glm::vec3 scale = to_vec3(scale_json);
    transform = glm::scale(transform, scale);

    return transform;
}

// TODO: Remove this struct, use rtr::vertex instead
struct vert
{
    std::array<float, 3> pos, nor;
    std::array<float, 2> uv;
};

struct triangle
{
    union {
        struct
        {
            uint32_t v0, v1, v2;
        };
        uint32_t vs[3];
    };
    uint32_t material;
};

static rtr::primitives::mesh load_mesh(const std::string& filename)
{
    std::ifstream input_file(filename, std::ios_base::in | std::ios_base::binary);
    if (!input_file.good())
        throw std::runtime_error("No such file called: " + filename);

    uint64_t num_vertices;
    std::vector<vert> vertices;

    input_file.read(reinterpret_cast<char*>(&num_vertices), sizeof num_vertices);
    vertices.resize(num_vertices);
    input_file.read(reinterpret_cast<char*>(vertices.data()), num_vertices * sizeof(vert));

    uint64_t num_triangles;
    std::vector<triangle> triangles;

    input_file.read(reinterpret_cast<char*>(&num_triangles), sizeof num_triangles);
    triangles.resize(num_triangles);
    input_file.read(reinterpret_cast<char*>(triangles.data()), num_triangles * sizeof(triangle));

    std::vector<rtr::primitives::face> faces;
    faces.reserve(num_triangles);
    for (auto& triangle : triangles)
    {
        std::array<rtr::vertex, 3> vert;
        for (int i = 0; i < 3; ++i)
        {
            auto& vertex = vertices[triangle.vs[i]];
            vert[i].poss = {vertex.pos[0], vertex.pos[1], vertex.pos[2]};
            vert[i].normal = {vertex.nor[0], vertex.nor[1], vertex.nor[2]};
            vert[i].u = vertex.uv[0];
            vert[i].v = vertex.uv[1];
        }

        faces.emplace_back(rtr::primitives::face{
            vert, rtr::primitives::face::normal_types::per_face, rtr::primitives::face::material_binding::per_object});
    }

    rtr::primitives::mesh mesh(std::move(faces), "wood");
    return mesh;
}

std::pair<std::string, rtr::material> load_material(const nlohmann::json& material_json)
{
    std::string name = material_json["name"];
    auto albedo_json = material_json["albedo"];

    glm::vec3 albedo;
    if (albedo_json.is_number())
    {
        albedo.x = albedo.y = albedo.z = float(albedo_json);
    } else
    {
        albedo.x = albedo_json[0];
        albedo.y = albedo_json[1];
        albedo.z = albedo_json[2];
    }

    return std::make_pair(name, rtr::material(albedo, {0.1, 0.1, 0.1}, {0, 0, 0}, {0, 0, 0}, 0, 0));
}

rtr::primitives::mesh load_quad(const glm::mat4x4& transform)
{
    std::vector<rtr::vertex> vertices = {rtr::vertex({-0.5f, 0.0f, -0.5f}),
                                         rtr::vertex({0.5f, 0.0f, -0.5f}),
                                         rtr::vertex({0.5f, 0.0f, 0.5f}),
                                         rtr::vertex({-0.5f, 0.0f, 0.5f})};

    for (auto& vertex : vertices) vertex.transform(transform);

    std::vector<rtr::primitives::face> faces;
    faces.emplace_back(rtr::primitives::face({vertices[0], vertices[1], vertices[2]}));
    faces.emplace_back(rtr::primitives::face({vertices[0], vertices[2], vertices[3]}));

    return rtr::primitives::mesh(faces, "quad");
}

rtr::scene load_mitsuba(const std::string& filename)
{
    rtr::scene scene;
    std::ifstream input_file(filename);
    nlohmann::json scene_json;
    input_file >> scene_json;

    /*
     * Load the renderer settings
     */
    auto renderer_settings = scene_json["renderer"];
    scene.progressive_render = false;
    scene.samples_per_pixel = renderer_settings["spp"];
    scene.output_file_name = renderer_settings["output_file"];
    scene.output_hdr_name = renderer_settings["hdr_output_file"];
    scene.scene_bvh = renderer_settings["scene_bvh"];
    // scene.max_recursion_depth = scene_json["integrator"]["max_bounces"];

    /*
     * Load the camera settings
     */
    auto camera_settings = scene_json["camera"];
    glm::vec3 position = to_vec3(camera_settings["transform"]["position"]);
    glm::vec3 lookat = to_vec3(camera_settings["transform"]["look_at"]);
    glm::vec3 up_dir = to_vec3(camera_settings["transform"]["up"]);

    auto lookat_dir = lookat - position;

    float fov = camera_settings["fov"];
    std::string pinhole = camera_settings["type"];

    auto foc = 1.f;
    auto image_plane_distance = 1.f;

    int width = camera_settings["resolution"];
    int height = camera_settings["resolution"];

    scene.camera =
        rtr::camera(position, lookat_dir, up_dir, foc, fov, width, height, image_plane_distance, pinhole == "pinhole");

    // std::cerr << width << '\n';
    // std::cerr << scene.camera.height << '\n';
    // std::cerr << scene.camera.is_pinhole() << '\n';

    /*
     * Load the materials
     */
    auto materials = scene_json["bsdfs"];
    std::map<std::string, rtr::material> all_materials;
    for (auto& material : materials)
    {
        auto mat = load_material(material);
        all_materials.insert(mat);
    }


    scene.lghts.emplace_back(glm::vec3{0, 0, 0}, glm::vec3{100, 100, 100});
    scene.lghts.emplace_back(glm::vec3{0, 0, 0}, glm::vec3{1, 1, 1});

    /*
     * Load the primitives
     */
    auto primitives = scene_json["primitives"];
    for (auto& primitive : primitives)
    {
        //     std::cerr << std::setw(4) << primitive << '\n';
        auto transform = get_transform_matrix(primitive["transform"]);
        auto type = primitive["type"];
        std::string material_id = primitive["bsdf"];
        auto mesh_material = all_materials[material_id];
        if (type == "mesh")
        {
            // if (primitive["file"])
            auto mesh = load_mesh("../Scenes/Mitsuba/veach-bidir/" + std::string(primitive["file"]));
            mesh.materials.push_back(mesh_material);
            scene.meshes.emplace_back(std::move(mesh));
        } else if (type == "quad")
        {
            auto quad = load_quad(transform);
            std::cerr << "hi\n";
            scene.meshes.emplace_back(std::move(quad));
        }

        if (primitive.find("power") != primitive.end())
        {
            auto power_json = primitive["power"];
            std::cerr << std::setw(4) << power_json << '\n';
            auto light_material = power_json.is_array() ? emissive(to_vec3(power_json)) : emissive(float(power_json));
            scene.meshes.back().materials.push_back(light_material);
        }
    }

    return scene;
}
} // namespace loaders
} // namespace rtr