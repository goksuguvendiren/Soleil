#pragma once

#include <optional>
#include <memory>
#include "photon.hpp"
#include <memory>
#include "KDTree.hpp"
#include <unordered_map>


//using boost::hash_combine
template <class T>
inline void hash_combine(std::size_t& seed, T const& v)
{
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
    template<typename T>
    struct hash<vector<T>>
    {
        typedef vector<T> argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& in) const
        {
            size_t size = in.size();
            size_t seed = 0;
            for (size_t i = 0; i < size; i++)
                //Combine the hash of the current vector with the hashes of the previous ones
                ::hash_combine(seed, in[i]);
            return seed;
        }
    };
}

namespace rtr
{   
    class ray;
    struct payload;

    class photon_map
    {
        mutable KDTree tree;
        std::unordered_map<point_t, photon> map;
    public:
        photon_map() = default;
        photon_map(const std::vector<rtr::photon>& photons);
        
        std::vector<photon> search(const glm::vec3& position, float d) const;        
    };
}
