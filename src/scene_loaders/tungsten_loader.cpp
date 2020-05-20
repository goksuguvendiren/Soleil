#include "materials/emissive.hpp"
#include "nlohmann/json.hpp"
#include "scene_loaders.hpp"
#include "utils.hpp"

#include "lights/area.hpp"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

namespace soleil
{
namespace loaders
{
static glm::mat4x4 get_transform_matrix(const nlohmann::json& transform_json)
{
    glm::mat4x4 transform = glm::mat4x4(1.0f);

    if (transform_json.empty())
        return transform;

    if (transform_json.contains("position"))
    {
        auto translate_json = transform_json["position"];

        glm::vec3 translation = to_vec3(translate_json);
        transform = glm::translate(transform, translation);
    }

    if (transform_json.contains("rotation"))
    {
        auto rotation_json = transform_json["rotation"];
        glm::vec3 rotation = to_vec3(rotation_json);
        transform = glm::rotate(transform, glm::radians(rotation.x), {1, 0, 0});
        transform = glm::rotate(transform, glm::radians(rotation.y), {0, 1, 0});
        transform = glm::rotate(transform, glm::radians(rotation.z), {0, 0, 1});
    }

    if (transform_json.contains("scale"))
    {
        auto scale_json = transform_json["scale"];
        glm::vec3 scale = to_vec3(scale_json);
        transform = glm::scale(transform, scale);
    }

    return transform;
}

// TODO: Remove this struct, use soleil::vertex instead
struct vert
{
    std::array<float, 3> pos;
    std::array<float, 3> nor;
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

static soleil::primitives::mesh load_mesh(const std::string& filename, const glm::mat4x4& transform)
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

    std::vector<soleil::primitives::face> faces;
    faces.reserve(num_triangles);
    for (auto& triangle : triangles)
    {
        std::array<soleil::vertex, 3> vert{};
        for (int i = 0; i < 3; ++i)
        {
            auto& vertex = vertices[triangle.vs[i]];
            vert[i].poss   = glm::vec3{vertex.pos[0], vertex.pos[1], vertex.pos[2]};
            vert[i].normal = glm::vec3{vertex.nor[0], vertex.nor[1], vertex.nor[2]};
            vert[i].m_uv   = glm::vec2{vertex.uv[0],  vertex.uv[1]};

            vert[i].transform(transform);
        }

        faces.emplace_back(soleil::primitives::face{
            vert, soleil::primitives::face::normal_types::per_vertex, soleil::primitives::face::material_binding::per_object});
    }

    soleil::primitives::mesh mesh(std::move(faces), "wood");
    return mesh;
}

std::tuple<std::string, std::unique_ptr<soleil::materials::base>, std::optional<soleil::materials::texture>>
    load_material(const nlohmann::json& material_json, const std::string& folder_path)
{
    static int default_id = 0;
    std::string name = "default_" + std::to_string(default_id++);
    if (material_json.find("name") != material_json.end())
        name = material_json["name"];

    auto albedo_json = material_json["albedo"];

    std::optional<soleil::materials::texture> opt_texture;

    glm::vec3 albedo;
    if (albedo_json.is_number())
    {
        albedo.x = albedo.y = albedo.z = float(albedo_json);
    }
    else if  (albedo_json.is_string())
    {
        auto texture_name = std::string(albedo_json);
        std::cerr << "Tungsten Loader | Loading texture : " << texture_name << '\n';

        opt_texture = soleil::materials::texture(folder_path + "/" + texture_name);
    }
    else if (albedo_json.is_object())
    {
        auto on_color = albedo_json["on_color"];
        albedo.x = on_color[0];
        albedo.y = on_color[1];
        albedo.z = on_color[2];
    }
    else
    {
        albedo.x = albedo_json[0];
        albedo.y = albedo_json[1];
        albedo.z = albedo_json[2];
    }

    return std::make_tuple(name,
        std::make_unique<soleil::materials::base>(soleil::materials::base(albedo, std::move(opt_texture), name)),
            opt_texture);
}

soleil::primitives::mesh load_quad(const glm::mat4x4& transform, const std::string& name)
{
    std::vector<soleil::vertex> vertices = {soleil::vertex({-0.5f, 0.0f, -0.5f}),
                                         soleil::vertex({0.5f, 0.0f, -0.5f}),
                                         soleil::vertex({0.5f, 0.0f, 0.5f}),
                                         soleil::vertex({-0.5f, 0.0f, 0.5f})};

    for (auto& vertex : vertices) vertex.transform(transform);

    std::vector<soleil::primitives::face> faces;
    faces.emplace_back(soleil::primitives::face({vertices[0], vertices[1], vertices[2]},
                                             soleil::primitives::face::normal_types::per_face, soleil::primitives::face::material_binding::per_object));
    faces.emplace_back(soleil::primitives::face({vertices[0], vertices[2], vertices[3]},
                                             soleil::primitives::face::normal_types::per_face, soleil::primitives::face::material_binding::per_object));

    return soleil::primitives::mesh(faces, name);
}

soleil::primitives::mesh load_cube(const glm::mat4x4 &transform, const std::string& name)
{
    std::vector<soleil::vertex> vertices = {
        // Top
        soleil::vertex({-0.5f, 0.5f, -0.5f}),
        soleil::vertex({0.5f, 0.5f, -0.5f}),
        soleil::vertex({0.5f, 0.5f, 0.5f}),
        soleil::vertex({-0.5f, 0.5f, 0.5f}),

        // Bottom
        soleil::vertex({-0.5f, -0.5f, -0.5f}),
        soleil::vertex({0.5f, -0.5f, -0.5f}),
        soleil::vertex({0.5f, -0.5f, 0.5f}),
        soleil::vertex({-0.5f, -0.5f, 0.5f}),

        // Right
        soleil::vertex({0.5f, 0.5f, 0.5f}),
        soleil::vertex({0.5f, 0.5f, -0.5f}),
        soleil::vertex({0.5f, -0.5f, -0.5f}),
        soleil::vertex({0.5f, -0.5f, 0.5f}),

        // Left
        soleil::vertex({-0.5f, 0.5f, -0.5f}),
        soleil::vertex({-0.5f, 0.5f, 0.5f}),
        soleil::vertex({-0.5f, -0.5f, 0.5f}),
        soleil::vertex({-0.5f, -0.5f, -0.5f}),

        // Back
        soleil::vertex({0.5f, 0.5f, -0.5f}),
        soleil::vertex({-0.5f, 0.5f, -0.5f}),
        soleil::vertex({-0.5f, -0.5f, -0.5f}),
        soleil::vertex({0.5f, -0.5f, -0.5f}),

        // Front
        soleil::vertex({-0.5f, 0.5f, 0.5f}),
        soleil::vertex({0.5f, 0.5f, 0.5f}),
        soleil::vertex({0.5f, -0.5f, 0.5f}),
        soleil::vertex({-0.5f, -0.5f, 0.5f}),
    };

    for (auto &vertex : vertices)
        vertex.transform(transform);

    std::vector<soleil::primitives::face> faces;
    for (int i = 0; i < vertices.size() / 4; i++)
    {
        faces.emplace_back(soleil::primitives::face({vertices[4 * i], vertices[4 * i + 1], vertices[4 * i + 2]},
                                                 soleil::primitives::face::normal_types::per_face, soleil::primitives::face::material_binding::per_object));
        faces.emplace_back(soleil::primitives::face({vertices[4 * i], vertices[4 * i + 2], vertices[4 * i + 3]},
                                                 soleil::primitives::face::normal_types::per_face, soleil::primitives::face::material_binding::per_object));
    }

    std::cerr << faces.size() << '\n';

    return soleil::primitives::mesh(faces, name);
}

soleil::light::area load_area_light(const soleil::primitives::mesh& quad, nlohmann::json& primitive)
{
    glm::vec3 intensity;
    if (primitive.find("power") != primitive.end()) intensity = to_vec3(primitive["power"]);
    else if (primitive.find("emission") != primitive.end())
    {
        std::cerr << primitive["emission"] << '\n';
        intensity = to_vec3(primitive["emission"]);
    }

    std::cerr << "load area light | intensity is: " << intensity << '\n';

    soleil::light::area area_light(quad, intensity);

    return area_light;
}

soleil::scene load_tungsten(const std::string &filename)
{
    auto last_slash = filename.find_last_of("/");
    auto folder_path = filename.substr(0, last_slash);

    // soleil::scene scene;
    soleil::scene_information info;
    std::ifstream input_file(filename);
    nlohmann::json scene_json;
    input_file >> scene_json;

    /*
     * Load the renderer settings
     */
    auto renderer_settings = scene_json["renderer"];
    info.progressive_render = false;
    info.samples_per_pixel = renderer_settings["spp"];
    info.output_file_name = renderer_settings["output_file"];
    info.output_hdr_name = renderer_settings["hdr_output_file"];
    // info.scene_bvh = renderer_settings["scene_bvh"];
    // scene.max_recursion_depth = scene_json["integrator"]["max_bounces"];

    std::cerr << renderer_settings["scene_bvh"] << '\n';

    /*
     * Load the camera settings
     */
    auto camera_settings = scene_json["camera"];
    glm::vec3 position = to_vec3(camera_settings["transform"]["position"]);
    glm::vec3 lookat = to_vec3(camera_settings["transform"]["look_at"]);
    glm::vec3 up_dir = to_vec3(camera_settings["transform"]["up"]);
    auto lookat_dir = glm::normalize(lookat - position);// - position;

    std::string pinhole = camera_settings["type"];

    auto resolution = camera_settings["resolution"];

    int width = 0, height = 0;
    if (resolution.is_array())
    {
        width = resolution[0];
        height = resolution[1];
    }
    else
    {
        width = height = (int)resolution;
    }

    float fov = camera_settings["fov"]; // HORIZONTAL FOV. WE WANT THE VERTICAL FOV.
    float vertical_fov = 2 * glm::degrees(glm::atan((height / (float)width) * glm::tan(glm::radians(fov / 2.f))));

    info.camera =
        soleil::camera(position, lookat_dir, up_dir, soleil::radians{glm::radians(vertical_fov)}, width, height, pinhole == "pinhole");

    /*
     * Load the materials
     */
    auto materials = scene_json["bsdfs"];
    std::vector<std::unique_ptr<soleil::materials::base>> all_materials;
    std::map<std::string, int> material_mappings; // name - index mapping of the materials
    for (auto &material : materials)
    {
        auto mat = load_material(material, folder_path);
        auto& [name, m, opt_tex] = mat;
        all_materials.push_back(std::move(m)); // put the material to the list, get its index, and put it into the map
        material_mappings.insert({name, all_materials.size() - 1});
    }

    /*
     * Load the primitives
     */
    auto primitives = scene_json["primitives"];
    int id = 0;
    for (auto &primitive : primitives)
    {
        //     std::cerr << std::setw(4) << primitive << '\n';
        auto transform = get_transform_matrix(primitive["transform"]);
        auto type = primitive["type"];
        std::string name = "";

        auto material_id = primitive["bsdf"];
        int mesh_material_idx = 0;
        if (material_id.is_string())
        {
            name = std::string(material_id);
            mesh_material_idx = material_mappings[material_id];
        }
        else if (material_id.is_object())
        {
            // that's a very basic bsdf that's inlined.
            auto [nm, material, _] = load_material(material_id, folder_path);
            all_materials.push_back(std::move(material)); // put the material to the list, get its index, and put it into the map
            material_mappings.insert({nm, all_materials.size() - 1});

            mesh_material_idx = material_mappings[nm];
            name = nm;
        }
        else
        {
            assert(false && "unknown material type");
        }

        std::cerr << "Tungsten Loader | Loading primitive : " << name << '\n';
        if (type == "mesh")
        {
            auto model_file = std::string(primitive["file"]);
            auto mesh = load_mesh(folder_path + "/" + std::string(model_file), transform);

            mesh.material_idx.push_back(mesh_material_idx);
            info.meshes.emplace_back(std::move(mesh));
        }
        else if (type == "quad")
        {
            auto quad = load_quad(transform, name);
            quad.material_idx.push_back(mesh_material_idx);

            // NOT an emissive quad, directly add as a mesh.
            if (primitive.find("power") == primitive.end() && primitive.find("emission") == primitive.end())
            {
                info.meshes.emplace_back(std::move(quad));
            }
            else
            {
                auto area_light = load_area_light(quad, primitive);
                info.area_lights.push_back(std::move(area_light));
            }
        }
        else if (type == "cube")
        {
            auto cube = load_cube(transform, name);
            cube.material_idx.push_back(mesh_material_idx);
            info.meshes.emplace_back(std::move(cube));
        }

        if (primitive.find("power") != primitive.end() && type != "quad")
        {
            auto power_json = primitive["power"];

            auto light_material = power_json.is_array() ? std::make_unique<materials::emissive>(materials::emissive(to_vec3(power_json))) : std::make_unique<materials::emissive>(materials::emissive(float(power_json)));
            all_materials.push_back(std::move(light_material));
            auto mat_idx = int(all_materials.size() - 1);
            info.meshes.back().material_idx.push_back(mat_idx);

            auto mesh_light = std::make_unique<primitives::emissive_mesh>(info.meshes.back().copy(), mat_idx);
            info.mesh_lights.push_back(std::move(mesh_light));
        }
        if (primitive.find("emission") != primitive.end() && type != "quad")
        {
            auto emission_json = primitive["emission"];

            auto light_material = emission_json.is_array() ? std::unique_ptr<materials::emissive>(new materials::emissive(to_vec3(emission_json))) : std::unique_ptr<materials::emissive>(new materials::emissive(float(emission_json)));
            all_materials.push_back(std::move(light_material));
            auto mat_idx = int(all_materials.size() - 1);
            info.meshes.back().material_idx.push_back(mat_idx);

            auto mesh_light = std::make_unique<primitives::emissive_mesh>(info.meshes.back().copy(), mat_idx);
            info.mesh_lights.push_back(std::move(mesh_light));
        }

//        std::cerr << "Mesh id: " << id++ << "\n";
//        std::cerr << "Material idx: " << info.meshes.back().material_idx[0] << "\n";
        // for (auto& vert : info.meshes.back().faces.)
        // std::cerr << "Normals: " << info.meshes.back().material_idx[0] << "\n";
    }
    // Add a point light source when there's none!
    if (info.total_light_size() == 0)
    {
        auto from = glm::vec3(-18.862, 69.2312, 69.651);
        auto to = glm::vec3(0, 0, 0);
        info.dir_lights.push_back(soleil::light::directional(to - from, {8.,8.,8.}));
    }
        //    info.lights.push_back(soleil::light::point({-18.862, 69.2312, 69.651}, {8000.0, 8000.0, 8000.0}));
//    info.lights.push_back(soleil::light::point({10, 30, 69.651}, {60000.0, 60000.0, 60000.0}));

    info.materials = std::move(all_materials);
    return soleil::scene(std::move(info));
}
} // namespace loaders
} // namespace soleil