#pragma once

#include <QString>

enum class FBBTeam
{
    // Error
    Unknown,

    // Teams
    LAA,
    HOU,
    OAK,
    TOR,
    ATL,
    MIL,
    STL,
    CHC,
    ARI,
    LAD,
    SF,
    CLE,
    SEA,
    MIA,
    NYM,
    WAS,
    BAL,
    SD,
    PHI,
    PIT,
    TEX,
    TB,
    BOS,
    CIN,
    COL,
    KC,
    DET,
    MIN,
    CHW,
    NYY,

    // Free agent
    FA,
};

QString TeamToString(const FBBTeam& team);