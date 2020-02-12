#include "materials/base.hpp"

#include "payload.hpp"
#include "scene.hpp"

glm::vec3 rtr::materials::base::shade(const rtr::scene& scene, const rtr::payload& pld) const
{
    auto amb = (1 - trans) * ambient * diffuse;
    glm::vec3 color = amb;

    scene.for_each_light([&pld, &color, this, &scene](const auto& light)
    {
        float epsilon = 1e-4;
        auto hit_position = pld.hit_pos + pld.hit_normal * epsilon;
//        rtr::ray shadow_ray = rtr::ray(hit_position, light->direction(hit_position), pld.ray.rec_depth + 1, false);
//        auto shadow = scene.shadow_trace(shadow_ray, light->distance(hit_position));
//
//        if (shadow.x <= epsilon && shadow.y <= epsilon && shadow.z <= epsilon)
//            return; // complete shadow, no need to compute the rest

        auto reflection_vector = reflect(light->direction(pld.hit_pos), pld.hit_normal);
        auto cos_angle = glm::dot(reflection_vector, -pld.ray.direction());
        auto highlight = std::max(0.f, std::pow(cos_angle, exp));

        auto cos_angle_diffuse = glm::dot(pld.hit_normal, light->direction(pld.hit_pos));
        auto diffuse =
            (1 - trans) * this->diffuse * std::max(cos_angle_diffuse, 0.0f);
        auto specular = this->specular * highlight;

        auto attenuation = light->attenuate(pld.hit_pos);

        color += (diffuse + specular) * light->color * attenuation;// * shadow;
    });

    return color;
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
