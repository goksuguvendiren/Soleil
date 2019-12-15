#include "scene.hpp"

#include "primitives/mesh.hpp"
#include "primitives/sphere.hpp"
#include "utils.hpp"

#include <memory>
#include <scene_io.h>
#include <vertex.hpp>

static glm::vec3 reflect(const glm::vec3& light, const glm::vec3& normal)
{
    return glm::normalize(2 * glm::dot(normal, light) * normal - light);
}

glm::vec3 refract(const glm::vec3& I, const glm::vec3& N, const float& ior)
{
    float cosi = std::clamp(glm::dot(I, N), -1.f, 1.f);
    float etai = 1, etat = ior;
    glm::vec3 n = N;
    if (cosi < 0)
    {
        cosi = -cosi;
    } else
    {
        std::swap(etai, etat);
        n = -N;
    }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return k < 0 ? glm::vec3{0, 0, 0} : eta * I + (eta * cosi - sqrtf(k)) * n;
}

std::optional<rtr::payload> rtr::scene::hit(const rtr::ray& ray) const
{
    std::optional<rtr::payload> min_hit = std::nullopt;

    for (auto& sphere : information.spheres)
    {
        auto hit = sphere.hit(ray);
        if (!hit)
            continue;
        if (!min_hit || hit->param < min_hit->param)
        {
            min_hit = *hit;
        }
    }

    for (auto& mesh : information.meshes)
    {
        auto hit = mesh.hit(ray);
        if (!hit)
            continue;
        if (std::isnan(hit->param))
            throw std::runtime_error("param is nan in scene::hit()!");

        if (!min_hit || hit->param < min_hit->param)
        {
            if (std::isnan(hit->param))
                throw std::runtime_error("param is nan in scene::hit() 2!");
            min_hit = *hit;
        }
    }

    if (min_hit && std::isnan(min_hit->param))
        throw std::runtime_error("param is nan in scene::hit() 3!");

    return min_hit;
}

// Do it recursively
glm::vec3 rtr::scene::shadow_trace(const rtr::ray& ray, float light_distance) const
{
    auto shadow = glm::vec3{1.f, 1.f, 1.f};
    std::optional<rtr::payload> pld = hit(ray);

    if (!pld)
        return shadow;
    if (pld && (pld->param < light_distance)) // some base case checks to terminate
    {
        auto& diffuse = pld->material->diffuse;
        auto normalized_diffuse = diffuse; // diffuse / std::max(std::max(diffuse.x, diffuse.y), diffuse.z);
        return shadow * normalized_diffuse * pld->material->trans;
    }

    auto hit_position = pld->hit_pos + ray.direction() * 1e-4f;
    rtr::ray shadow_ray = rtr::ray(hit_position, ray.direction(), ray.rec_depth, false);

    return shadow * shadow_trace(shadow_ray, light_distance - glm::length(pld->hit_pos - ray.origin()));
}

glm::vec3 rtr::scene::photon_trace(const rtr::ray& photon_ray) const
{}

void rtr::scene::print() const
{
    std::cerr << "Camera : \n";
    std::cerr << "\t Pos : " << information.camera.center() << '\n';
    std::cerr << "Meshes : \n";
    int i = 0;
    for (auto& m : information.meshes)
    {
        std::cerr << "\t\t Mesh " << i++ << " : \n";
        std::cerr << "\t\t Material " << m.materials[0].diffuse << " : \n";
        std::cerr << "\t\t " << m.materials[0].ambient << " : \n";
        std::cerr << "\t\t " << m.materials[0].specular << " : \n";
        for (auto& face : m.faces)
        {
            std::cerr << "\t\t\t Position_a : " << face.vertices[0].position() << '\n';
            std::cerr << "\t\t\t Position_b : " << face.vertices[1].position() << '\n';
            std::cerr << "\t\t\t Position_c : " << face.vertices[2].position() << '\n';
        }
    }
    std::cerr << "Spheres : \n";
    i = 0;
    for (auto& s : information.spheres)
    {
        std::cerr << "\t\t Sphere " << i++ << " : \n";
        std::cerr << "\t\t Material " << s.materials[0].diffuse << " : \n";
        std::cerr << "\t\t " << s.materials[0].ambient << " : \n";
        std::cerr << "\t\t " << s.materials[0].specular << " : \n";
        //        for (auto& face : m.faces)
        {
            std::cerr << "\t\t\t Center : " << s.origin << '\n';
            std::cerr << "\t\t\t Radius : " << s.radius << '\n';
        }
    }
}
