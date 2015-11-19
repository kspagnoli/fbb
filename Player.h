#pragma once

#include <string>

//------------------------------------------------------------------------------
// Player 
//------------------------------------------------------------------------------
struct Player
{
    // Flags
    static const int32_t Undrafted = -1;

    // Player Status
    enum Status
    {
        Active,
        Injured,
        Minors,
    };

    // player data
    std::string name;
    std::string team;

    // status
    Status status = Active;

    // fantasy
    int32_t ownerId = Undrafted;
    float paid = 0;

    // estimate
    float zScore = 0;
    float cost = 0;

    // comment
    std::string comment;
};

