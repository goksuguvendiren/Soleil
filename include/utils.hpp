#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <random>
#include <glm/gtc/quaternion.hpp>
#include "nlohmann/json.hpp"

#pragma once

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& vec)
{
    os << vec.x << ", " << vec.y << ", " << vec.z;
    return os;
}

inline std::random_device rd;  // Will be used to obtain a seed for the random number engine
inline std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
inline float get_random_float()
{
    std::uniform_real_distribution<float> dis(0, 1.0);
    return dis(gen);
}

inline float get_random_float(float min, float max)
{
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

inline int get_random_int(int min, int max) // box inclusive
{
    static std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<int> dist(min, max); // distribution in range [1, 6]

    return dist(rng);
}

inline glm::vec3 sample_hemisphere(int ms_id, int max_ms)
{
    static std::mt19937 rng(0);
    static std::uniform_real_distribution<float> dist1(0, 1);
    static std::uniform_real_distribution<float> dist2(0, 1);

    static constexpr double pi = 3.14159265358979323846;

    auto ksi1 = dist1(rng); // costheta, y coordinate
    auto ksi2 = dist2(rng);
    //    return sample_cosine(ksi1, ksi2);

    /*ksi1 = lerp(float(ms_id) / max_ms, float(ms_id + 1) / max_ms, ksi1);
     ksi2 = lerp(float(ms_id) / max_ms, float(ms_id + 1) / max_ms, ksi2);*/

    auto sin_theta = std::sqrt(1 - ksi1 * ksi1);
    auto phi = 2 * pi * ksi2;

    auto x = sin_theta * std::cos(phi);
    auto z = sin_theta * std::sin(phi);

    return glm::vec3{x, ksi1, z};

    auto v = glm::vec3(std::cos(2 * pi * ksi2) * std::sqrt(1 - ksi1 * ksi1), ksi1,
                       std::sin(2 * pi * ksi2) * std::sqrt(1 - ksi2 * ksi2));
    return v;
}

struct orthonormal_basis
{
    glm::vec3 x;
    glm::vec3 y;
    glm::vec3 z;
};

inline orthonormal_basis create_onb(const glm::vec3& normal)
{
    // create am orthonormal basis whose up(y) direction is aligned with the normal
    glm::vec3 n_t = {0, 0, 0}; // z coord
    if (std::fabs(normal.x) > std::fabs(normal.y))
        n_t = glm::vec3(normal.z, 0, -normal.x) / std::sqrt(normal.x * normal.x + normal.z * normal.z);
    else
        n_t = glm::vec3(0, -normal.z, -normal.y) / std::sqrt(normal.y * normal.y + normal.z * normal.z);

    auto n_b = glm::cross(normal, n_t); // x coord

    return orthonormal_basis{n_b, normal, n_t};
}

inline glm::vec3 sample_hemisphere(const glm::vec3& hit_normal, int ms_id = 0, int max_ms = 0)
{
    auto base_sample = sample_hemisphere(ms_id, max_ms); // sample hemisphere in the tangent space

    auto basis = create_onb(hit_normal);

    return glm::vec3{base_sample.x * basis.x.x + base_sample.y * basis.y.x + base_sample.z * basis.z.x,
                     base_sample.x * basis.x.y + base_sample.y * basis.y.y + base_sample.z * basis.z.y,
                     base_sample.x * basis.x.z + base_sample.y * basis.y.z + base_sample.z * basis.z.z};
}

inline bool hasEnding(std::string const& fullString, std::string const& ending)
{
    if (fullString.length() >= ending.length())
    {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    } else
    {
        return false;
    }
}

inline glm::vec3 to_vec3(float* vert)
{
    return {vert[0], vert[1], vert[2]};
}


inline glm::vec3 to_vec3(nlohmann::json& vert)
{
    std::cerr << "vert.is_array(): " << vert.is_array() << '\n';
    if (vert.is_array())
        return {float(vert[0]), float(vert[1]), float(vert[2])};

    std::cerr << "vert.is_number_float(): " << vert.is_number_float() << '\n';
    if (vert.is_number_float())
        return {float(vert), float(vert), float(vert)};
}
