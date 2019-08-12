//
// Created by Göksu Güvendiren on 2019-05-09.
//

#pragma once

#include <functional>
#include <string>
#include <optional>
#include <glm/vec3.hpp>
#include <material.hpp>
#include "ray.hpp"

namespace rtr
{
    struct payload;
    namespace primitives
    {
        
        class sphere
        {
        public:
            sphere(const std::string& nm, const glm::vec3& pos, float r) :
                   name(nm), origin(pos), radius(r) {}

            sphere(const std::string& nm, const glm::vec3& pos, float r, const rtr::material& m) :
            name(nm), origin(pos), radius(r)
            {
                materials.push_back(m);
            }

            std::vector<material> materials;
            std::optional<rtr::payload> hit(const rtr::ray& ray) const;

        // private:
            std::string name;
            int id;

            glm::vec3 origin;
            float radius;
        };
    }
}
