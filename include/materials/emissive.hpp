#include "materials/base.hpp"
#include "photon.hpp"

#include <glm/glm.hpp>
#include <payload.hpp>

namespace soleil
{
namespace materials
{
class emissive : public base
{
public:
    emissive(float p)
        : power(p, p, p)
    {}

    emissive(const glm::vec3& p)
        : power(p)
    {}

    virtual bool is_emissive() const
    {
        return true;
    }

    glm::vec3 f(const scene& scene, const payload& pld, const glm::vec3& light_direction) const
    {
        // std::cerr << "hit the light jack" << '\n';
        return power;
    }

private:
    glm::vec3 power;
};
} // namespace materials
} // namespace soleil