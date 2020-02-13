#include "materials/base.hpp"

#include "payload.hpp"
#include "scene.hpp"

glm::vec3 rtr::materials::base::f(const rtr::scene& scene, const rtr::payload& pld) const
{
    return diffuse / glm::pi<float>();
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

glm::vec3 rtr::materials::base::sample(const glm::vec3& hit_normal, const payload& pld) const
{
    glm::vec3 sample_direction;
//    if (trans > 0)
//    {
//        sample_direction = refract(pld.ray.direction(), hit_normal, refr_index);
//    }
//    else
//    {
        sample_direction = sample_hemisphere(hit_normal);
//    }
    return sample_direction;
}
