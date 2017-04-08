#pragma once

struct FBBLeaugeSettings 
{
    struct Leauge
    {
        enum class Type
        {
            Mixed,
            NL,
            AL,

        } type = Type::Mixed;

    } leauge;

    struct Categories
    {
        struct Hitting
        {
            bool AVG = true;
            bool RBI = true;
            bool R = true;
            bool SB = true;
            bool HR = true;

        } hitting;

        struct Pitching
        {
            bool W = true;
            bool SV = true;
            bool ERA = true;
            bool WHIP = true;
            bool SO = true;

        } pitching;

    } categories;

    struct Positions
    {
        struct Hitting 
        {
            uint32_t numC = 2;
            uint32_t num1B = 1;
            uint32_t num2B = 1;
            uint32_t numSS = 1;
            uint32_t num3B = 1;
            uint32_t numOF = 5;
            uint32_t numMI = 1;
            uint32_t numCI = 1;
            uint32_t numIF = 1;
            uint32_t numUT = 1;

        } hitting;

        struct Pitching
        {
            uint32_t numSP = 0;
            uint32_t numRP = 0;
            uint32_t numP = 10;

        } pitching;

        uint32_t numBench = 3;

    } positions;

};