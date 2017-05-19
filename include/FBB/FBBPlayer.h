#pragma once

#include "FBB/FBBTeam.h"
#include "FBB/FBBPosition.h"

#include <QString>

#include <cstdint>
#include <memory>
#include <array>

using FBBPlayerId = QString;
using FBBOwnerId = uint64_t;

class FBBPlayer
{
public:

    enum HittingStats
    {
        HITTING_STAT_PA,
        HITTING_STAT_AB,
        HITTING_STAT_H,
        HITTING_STAT_2B,
        HITTING_STAT_3B,
        HITTING_STAT_HR,
        HITTING_STAT_R,
        HITTING_STAT_RBI,
        HITTING_STAT_BB,
        HITTING_STAT_SO,
        HITTING_STAT_HBP,
        HITTING_STAT_SB,
        HITTING_STAT_CS,

        HittingStats_Count
    };

    enum PitchingStats
    {
        PITCHING_STAT_W,
        PITCHING_STAT_L,
        PITCHING_STAT_GS,
        PITCHING_STAT_G,
        PITCHING_STAT_SV,
        PITCHING_STAT_IP,
        PITCHING_STAT_H,
        PITCHING_STAT_ER,
        PITCHING_STAT_HR,
        PITCHING_STAT_SO,
        PITCHING_STAT_BB,

        PitchingStats_Count
    };

    FBBPlayerId id;

    QString name;
    FBBTeam team = FBBTeam::Unknown;
    uint32_t age = 0;
    uint32_t experience = 0;
    FBBPositionMask eligablePositions = FBB_POSITION_UNKNOWN;

    struct Projection
    {
        enum Type
        {
            PROJECTION_TYPE_UNKNONWN = 0,
            PROJECTION_TYPE_HITTING = 1 << 1,
            PROJECTION_TYPE_PITCHING = 1 << 2,
            PROJECTION_TYPE_ANY = PROJECTION_TYPE_HITTING | PROJECTION_TYPE_PITCHING,
        };

        Type type = PROJECTION_TYPE_UNKNONWN;
        using TypeMask = uint8_t;

        union 
        {
            // std::array<double, HittingStats_Count> hitting;

            struct Hitting
            {
                uint32_t PA;
                uint32_t AB;
                uint32_t H;
                uint32_t _2B;
                uint32_t _3B;
                uint32_t HR;
                uint32_t R;
                uint32_t RBI;
                uint32_t BB;
                uint32_t SO;
                uint32_t HBP;
                uint32_t SB;
                uint32_t CS;

                // uint32_t AVG;
                // uint32_t OBP;
                // uint32_t SLG;
                // uint32_t OPS;
                // uint32_t wOBA;
                // uint32_t wRC;
                // uint32_t BsR;
                // uint32_t Fld;
                // uint32_t Off;
                // uint32_t Def;
                // uint32_t WAR;

                float AVG() const
                {
                    return AB > 0 ? H / float(AB) : 0.f;
                }

            } hitting;

            struct Pitching
            {
                uint32_t W;
                uint32_t L;
                uint32_t GS;
                uint32_t G;
                uint32_t SV;
                uint32_t IP;
                uint32_t H;
                uint32_t ER;
                uint32_t HR;
                uint32_t SO;
                uint32_t BB;

                // uint32_t ERA;
                // "WHIP"
                // "K/9"
                // "BB/9"
                // "FIP"
                // "WAR"
                // "RA9-WAR"

                float ERA() const
                {
                    return IP > 0 ? (9*ER / float(IP)) : 0.f;
                }

                float WHIP() const
                {
                    return IP > 0 ? (BB + H) / float(IP) : 0.f;
                }
            } pitching;
        };

    };

    struct Calculations
    {
        struct {
            float AVG;
            float RBI;
            float R;
            float SB;
            float HR;
        } zHitting;

        struct {
            float W;
            float SV;
            float ERA;
            float WHIP;
            float SO;
        } zPitching;

        float zScore = 0;
        float estimate = 0;
        uint32_t rank = 0;

    } calculations;

    struct DraftInfo
    {
        FBBOwnerId owner = 0;
        uint32_t paid = 0;
        FBBPositionBits position = FBB_POSITION_UNKNOWN;
    } draftInfo;

    std::unique_ptr<Projection> spProjection;
};