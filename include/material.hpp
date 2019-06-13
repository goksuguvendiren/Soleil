//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include "utils.hpp"

namespace rtr
{
    class scene;
    class payload;

    enum class PathType
    {
        Absorbed,
        Transmitted,
        Reflected
    };

    class material
    {
    public:
        material() = default;
        material(const glm::vec3& diff, const glm::vec3& amb, const glm::vec3& spec, const glm::vec3& ems, float p, float t) : diffuse(diff), ambient(amb), specular(spec), emissive(ems), exp(p), trans(t), refr_index(1.0f)
        {
            if (trans > 0.f) refr_index = 1.5f;
        }
        
        glm::vec3 shade(const scene& scene, const payload& pld) const;

        rtr::PathType russian_roulette() const 
        {
            auto rr = get_random_float(0, 1);
            if (rr < 0.1)
                return PathType::Absorbed;
            else if (rr < specular.x)
                return PathType::Reflected;
            else if (rr < trans)
                return PathType::Transmitted;
            return PathType::Absorbed;
        }

        glm::vec3 sample(const glm::vec3& hit_normal) const;
        
        glm::vec3 diffuse;
        glm::vec3 ambient;
        glm::vec3 specular;
        glm::vec3 emissive;
        
        float exp;
        float trans;
        float refr_index;
        
    private:

        
        
    };
}
