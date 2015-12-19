#pragma once

#include "Player.h"

//------------------------------------------------------------------------------
// Pitcher
//------------------------------------------------------------------------------
struct Pitcher : Player
{
    enum class Position
    {
        None    = 0 << 0,
        Starter = 1 << 1,
        Relief  = 1 << 2,
    };
    using PositionMask = uint32_t;

    // Positions
    PositionMask positions = PositionMask(Position::None);

    // Stats
    float IP = 0;
    uint32_t SO = 0;
    uint32_t W = 0;
    uint32_t SV = 0;
    float ERA = 0;
    float WHIP = 0;

    // zScores
    float zSO = 0;
    float zW = 0;
    float zSV = 0;
    float zERA = 0;
    float zWHIP = 0;

    // weighted score
    float wERA = 0;
    float wWHIP = 0;
};
