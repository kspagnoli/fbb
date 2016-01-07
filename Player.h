#pragma once

#include <QString>

//------------------------------------------------------------------------------
// Player 
//------------------------------------------------------------------------------
struct Player
{
    // Possible positions
    enum Position
    {
        // Unknown (or not set)
        None     = 0,

        // Hitters
        Catcher  = 1 << 1,
        First    = 1 << 2,
        Second   = 1 << 3,
        SS       = 1 << 4,
        Third    = 1 << 5,
        Outfield = 1 << 6,
        DH       = 1 << 7,

        // Pitchers
        Starter  = 1 << 20,
        Relief   = 1 << 21,
    };

    // Position mask
    using PositionMask = uint32_t;

    enum Catergory
    {
        Hitter  = 1 << 1,
        Pitcher = 1 << 2, 
    };
    
    // CatergoryMask
    using CatergoryMask = uint32_t;

    // Player data
    uint32_t index;
    QString name;
    QString team;                               // TODO: change to ID
    CatergoryMask catergory;
    PositionMask eligiblePositions = Position::None;

    // Player Status
    enum Status
    {
        Majors,
        Minors,
        Injured,
        Drafted,
    };

    // Status
    Status status = Status::Majors;

    // Fantasy
    int32_t ownerId = 0;
    float paid = 0;
    QString draftPosition;

    // Cost estimate
    float zScore = 0;
    float cost = 0;

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
