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
    enum class Status
    {
        Majors,
        Minors,
        Injured,
        Drafted,
    };

    // Player data
    std::string name;
    std::string team;

    // Status
    Status status = Status::Majors;

    // Fantasy
    int32_t ownerId = Undrafted;
    float paid = 0;

    // Estimate
    float zScore = 0;
    float cost = 0;

    // Comment
    std::string comment;
};
