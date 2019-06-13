#include <vector>
#include <random>
#include <optional>
#include <iostream>
#include <algorithm>
#include "photon_map.hpp"
#include "ray.hpp"
#include "photon.hpp"
#include "payload.hpp"
#include "utils.hpp"

rtr::photon_map::photon_map(const std::vector<rtr::photon>& photons)
{
    pointVec poss(photons.size());
    std::transform(photons.begin(), photons.end(), poss.begin(), [](auto& ph) {
        return std::vector<double>{ ph.origin().x, ph.origin().y, ph.origin().z };
    });
    std::transform(photons.begin(), photons.end(), std::inserter(map, map.end()), [](auto& ph) {
        return std::make_pair(std::vector<double>{ ph.origin().x, ph.origin().y, ph.origin().z }, ph);
    });
    tree = KDTree(std::move(poss));
}

std::vector<rtr::photon> rtr::photon_map::search(const glm::vec3& position, float d) const
{
    auto res = tree.neighborhood_points(std::vector<double>{ position.x, position.y, position.z }, d);
    std::vector<rtr::photon> return_vec;
    return_vec.reserve(res.size());
    std::transform(res.begin(), res.end(), std::back_inserter(return_vec), [&](auto& pt){ 
        return map.find(pt)->second;
    });
    return return_vec;
}
