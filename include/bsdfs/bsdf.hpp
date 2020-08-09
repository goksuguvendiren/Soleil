//
// Created by goksu on 6/22/20.
//

#pragma once

#include <utils.hpp>

namespace soleil::bsdfs
{
class utils
{
public:
    // global->local
    static glm::vec3 world_to_local(const orthonormal_basis& onb, const glm::vec3& vector)
    {
        return glm::normalize(glm::vec3(glm::dot(onb.x, vector), glm::dot(onb.y, vector), glm::dot(onb.z, vector)));
    }

    static float cos_theta(const glm::vec3& vec)
    {
        return vec.y;
    }

    static float cos_2_theta(const glm::vec3& vec)
    {
        return vec.y * vec.y;
    }

    static float sin_2_theta(const glm::vec3& vec)
    {
        return std::max(0.f, 1.f - cos_2_theta(vec));
    }

    static float sin_theta(const glm::vec3& vec)
    {
        return std::sqrt(sin_2_theta(vec));
    }

    static float tan_2_theta(const glm::vec3& vec)
    {
        return sin_2_theta(vec) / cos_2_theta(vec);
    }

    static float tan_theta(const glm::vec3& vec)
    {
        return sin_theta(vec) / cos_theta(vec);
    }

    static float cos_phi(const glm::vec3& vec)
    {
        auto sin_theta = utils::sin_theta(vec);
        if (sin_theta == 0.f)
            return 1.f;
        return std::clamp(vec.x / sin_theta, -1.f, 1.f);
    }

    static float cos_2_phi(const glm::vec3& vec)
    {
        return cos_phi(vec) * cos_phi(vec);
    }

    static float sin_phi(const glm::vec3& vec)
    {
        auto sin_theta = utils::sin_theta(vec);
        if (sin_theta == 0.f)
            return 0.f;
        return std::clamp(vec.z / sin_theta, -1.f, 1.f);
    }

    static float sin_2_phi(const glm::vec3& vec)
    {
        return sin_phi(vec) * sin_phi(vec);
    }
};
} // namespace soleil::bsdfs