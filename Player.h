#pragma once

#include <QString>

//------------------------------------------------------------------------------
// PlayerPosition 
// TODO: Just use a bitfield here...
//------------------------------------------------------------------------------
enum class PlayerPosition
{
    None,
    Catcher,
    First,
    Second,
    SS,
    Third,
    Outfield,
    DH,
    Pitcher,

    MiddleInfield,
    CornerInfield,
    Utility,

    Starter,
    Relief,

    COUNT,
};

using PlayerPositionBitfield = uint32_t;

inline PlayerPositionBitfield ToBitfield(const PlayerPosition& position)
{
    return 1 << PlayerPositionBitfield(position);
}

inline PlayerPositionBitfield ToBitfield(const std::initializer_list<PlayerPosition>& positions)
{
    PlayerPositionBitfield bitfield = 0;
    for (auto& position : positions) {
        bitfield |= (1 << PlayerPositionBitfield(position));
    }
    return bitfield;
}

inline PlayerPositionBitfield AddFauxPositions(const PlayerPositionBitfield& positions)
{
    PlayerPositionBitfield ret = positions;
    if (positions & ToBitfield(PlayerPosition::SS) || positions & ToBitfield(PlayerPosition::Second)) {
        ret |= ToBitfield(PlayerPosition::MiddleInfield);
    }

    if (positions & ToBitfield(PlayerPosition::First) || positions & ToBitfield(PlayerPosition::Third)) {
        ret |= ToBitfield(PlayerPosition::CornerInfield);
    }

    if (positions & ToBitfield(PlayerPosition::Catcher) ||
        positions & ToBitfield(PlayerPosition::First) ||
        positions & ToBitfield(PlayerPosition::Second) ||
        positions & ToBitfield(PlayerPosition::SS) ||
        positions & ToBitfield(PlayerPosition::Third) ||
        positions & ToBitfield(PlayerPosition::Outfield))
    {
        ret |= ToBitfield(PlayerPosition::Utility);
    }

    return ret;
}

//------------------------------------------------------------------------------
// Player 
//------------------------------------------------------------------------------
struct Player
{
    enum Catergory
    {
        Hitter  = 1 << 1,
        Pitcher = 1 << 2, 
    };
    
    // CatergoryMask
    using CatergoryMask = uint32_t;

    enum Flag
    {
        FLAG_NONE,
        FLAG_STAR,
        FLAG_WATCH,
        FLAG_AVOID,

        FLAG_COUNT,
    };

    // User flag
    uint32_t flag = FLAG_NONE;

    // Player data
    uint32_t index;
    QString id;        // player id
    QString name;      
    QString team;     // TODO: change to ID
    uint32_t age = 0;
    uint32_t experience = 0;
    CatergoryMask catergory;
    PlayerPositionBitfield eligiblePositionBitfield = uint32_t(0);

    // Fantasy
    int32_t ownerId = 0;
    float paid = 0;
    PlayerPosition draftPosition = PlayerPosition::None;

    // Cost estimate category 
    uint32_t categoryRank = 0;
    float zScoreBonus = 0;
    float zScore = 0;
    float cost = 0;

    // Hitting stats
    struct HittingStats
    {
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

        // weighted zScores
        float wAVG = 0;

        // SPG
        float spgAVG = 0;
        float spgR = 0;
        float spgHR = 0;
        float spgRBI = 0;
        float spgSB = 0;

    } hitting;

    // Pitching stats
    struct PitchingStats
    {
        // Stats
        float IP = 0;
        float ER = 0;
        uint32_t H = 0;
        uint32_t BB = 0;
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

    } pitching;

    // Comment
    QString comment;
};
