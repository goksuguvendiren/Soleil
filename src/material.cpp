#include "materials/base.hpp"

#include "payload.hpp"
#include "scene.hpp"


static glm::vec3 reflect(const glm::vec3& light, const glm::vec3& normal)
{
    return glm::normalize(2 * glm::dot(normal, light) * normal - light);
}

glm::vec3 soleil::materials::base::f(const soleil::scene& scene, const soleil::payload& pld, const glm::vec3& light_direction) const
{
    glm::vec3 albedo = diffuse;
    if (texture)
        albedo = texture->getColor(pld.texture_coords);

    auto lambertian = albedo / glm::pi<float>();

    return lambertian;
//
//    auto n = exp;
//
//    auto reflected_direction = reflect(light_direction, pld.hit_normal);
//    auto cos_alpha = std::max(0.f, glm::dot(reflected_direction, -pld.ray.direction()));
//
//    auto modified_phong = specular * ((n+2) / (2.f * glm::pi<float>())) * std::pow(cos_alpha, n);
//
////    std::cerr << "specular: " << specular << '\n';
////    std::cerr << "std::pow(cos_alpha, n): " << std::pow(cos_alpha, n) << '\n';
////    std::cerr << "modified_phong: " << modified_phong << '\n';
//
//    return lambertian + modified_phong * 10.f; // lambertian + modified phong model
}

static glm::vec3 refract(const glm::vec3& I, const glm::vec3& N, const float& ior)
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

glm::vec3 soleil::materials::base::sample(const glm::vec3& hit_normal, const payload& pld) const
{
    return sample_hemisphere(hit_normal);
}
