#include "materials/base.hpp"

#include "payload.hpp"
#include "scene.hpp"

glm::vec3 soleil::materials::base::f(const soleil::scene& scene, const soleil::payload& pld) const
{
    glm::vec3 albedo = diffuse;
    if (m_texture)
        albedo = m_texture->get_sample(pld.texture_coords);

    return albedo / glm::pi<float>(); // lambertian model
}

// TODO: Move this to the correct place
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
