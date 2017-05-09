#pragma once

#include <QString>

enum FBBPositionBits
{
    // Default
    FBB_POSITION_UNKNOWN = 0,

    // Hitting Positions
    FBB_POSITION_C  = 1 << 1,
    FBB_POSITION_1B = 1 << 2,
    FBB_POSITION_2B = 1 << 3,
    FBB_POSITION_SS = 1 << 4,
    FBB_POSITION_3B = 1 << 5,
    FBB_POSITION_RF = 1 << 6,
    FBB_POSITION_CF = 1 << 7,
    FBB_POSITION_LF = 1 << 8,
    FBB_POSITION_DH = 1 << 9,

    // Pitching Positions
    FBB_POSITION_SP = 1 << 10,
    FBB_POSITION_RP = 1 << 11,

    // Aggregate positions
    FBB_POSITION_CI = FBB_POSITION_1B | FBB_POSITION_3B,
    FBB_POSITION_MI = FBB_POSITION_2B | FBB_POSITION_SS,
    FBB_POSITION_IF = FBB_POSITION_CI | FBB_POSITION_MI,
    FBB_POSITION_OF = FBB_POSITION_CF | FBB_POSITION_LF | FBB_POSITION_RF,
    FBB_POSITION_U = FBB_POSITION_C | FBB_POSITION_IF | FBB_POSITION_OF | FBB_POSITION_DH,
    FBB_POSITION_P = FBB_POSITION_SP | FBB_POSITION_RP,
};

using FBBPositionMask = uint32_t;

QString FBBPositionMaskToString(const FBBPositionMask& mask);
QStringList FBBPositionMaskToStringList(const FBBPositionMask& mask);
