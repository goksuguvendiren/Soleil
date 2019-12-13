#include "material.hpp"

#include <glm/glm.hpp>

namespace rtr
{
class emissive : public material
{
public:
    emissive(float p)
        : power(p, p, p)
    {}

    emissive(const glm::vec3& p)
        : power(p)
    {}

    bool is_emissive() const
    {
        return true;
    }

    glm::vec3 shade(const scene& scene, const payload& pld) const
    {
        std::cerr << "hit the light jack" << '\n';
        return power;
    }

private:
    glm::vec3 power;
};
} // namespace rtr