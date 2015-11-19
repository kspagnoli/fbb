#pragma once

#include "Player.h"

//------------------------------------------------------------------------------
// Hitter 
//------------------------------------------------------------------------------
struct Hitter : public Player
{
    enum class Position
    {
        None     = 0,
        Catcher  = 1 << 1,
        First    = 1 << 2,
        Second   = 1 << 3,
        SS       = 1 << 4,
        Third    = 1 << 5,
        Outfield = 1 << 6,
        DH       = 1 << 7,
    };
    using PositionMask = uint32_t;

    // positions
    PositionMask positions = PositionMask(Position::None);

    // sub-stat
    uint32_t PA = 0;
    uint32_t AB = 0;
    uint32_t H = 0;

    // core stats
    float AVG = 0;
    uint32_t R = 0;
    uint32_t RBI = 0;
    uint32_t HR = 0;
    uint32_t SB = 0;

    // zScores
    float zAVG = 0;
    float zR = 0;
    float zHR = 0;
    float zRBI = 0;
    float zSB = 0;
};
