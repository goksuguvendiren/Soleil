//
// Created by goksu on 6/7/19.
//

#pragma once

#include <glm/vec3.hpp>

namespace rtr
{
    class photon
    {
    public:
    	photon(const glm::vec3& p, const glm::vec3& o, const glm::vec3& d) : pw(p), org(o), dir(d) {}

        glm::vec3 power() const { return pw; }
        glm::vec3 origin() const { return org; }
        glm::vec3 direction() const { return dir; }

    private:
        glm::vec3 pw;
        glm::vec3 org;
        glm::vec3 dir;
    };
}