#pragma once

#include "Teams.h"

#include <cstdint>
#include <string>
#include <tuple>

#include <boost/functional/hash.hpp>

namespace std {

template<typename... T>
struct hash<tuple<T...>>
{
    size_t operator()(tuple<T...> const& arg) const noexcept
    {
        return boost::hash_value(arg);
    }
};

}

struct Appearances
{
    uint8_t G    = 0;
    uint8_t GS   = 0;
    uint8_t atC  = 0;
    uint8_t at1B = 0;
    uint8_t at2B = 0; 
    uint8_t at3B = 0;
    uint8_t atSS = 0;
    uint8_t atOF = 0;
    uint8_t atDH = 0;

    uint8_t age = 0;
    uint8_t exp = 0;
};

class PlayerApperances
{
public:
    PlayerApperances(const std::string& filename);
    const Appearances& Lookup(const std::string& name) const;

private:

    using PlayerKey = std::tuple<std::string, std::string>;
    std::unordered_map<PlayerKey, Appearances> m_mapAppearances;
};

