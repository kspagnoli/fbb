#pragma once

#include <QString>

#include <bitset>

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
    FBB_POSITION_CI = 1 << 12,
    FBB_POSITION_MI = 1 << 13,
    FBB_POSITION_IF = 1 << 14,
    FBB_POSITION_OF = 1 << 15,
    FBB_POSITION_U  = 1 << 16,
    FBB_POSITION_P  = 1 << 17,
};

enum 
{
    FBBPositionBitCount = 17,
};

using FBBPositionMask = uint32_t;

QString FBBPositionToString(FBBPositionBits bit);
QString FBBPositionMaskToString(FBBPositionMask mask, bool includeAggregates);
QStringList FBBPositionMaskToStringList(FBBPositionMask mask, bool includeAggregates);
bool IsAgregatePosition(FBBPositionBits position);