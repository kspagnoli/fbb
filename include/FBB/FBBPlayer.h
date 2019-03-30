#pragma once

#include "FBB/FBBTeam.h"
#include "FBB/FBBPosition.h"
#include "FBB/FBBLeaugeSettings.h"

#include <QString>
#include <QObject>

#include <cstdint>
#include <memory>
#include <array>

using FBBPlayerId = QString;
using FBBOwnerId = uint64_t;

class FBBPlayer : public QObject
{
    Q_OBJECT

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

    enum Fielding
    {
        FIELDING_C,
        FIELDING_1B,
        FIELDING_2B,
        FIELDING_SS,
        FIELDING_3B,
        FIELDING_LF,
        FIELDING_CF,
        FIELDING_RF,

        Fielding_Count
    };

    enum PlayerType
    {
        PLAYER_TYPE_HITTER,
        PLAYER_TYPE_PITCHER,
    };

    FBBPlayer(PlayerType type, const FBBPlayerId& id, const QString& name,  QObject* pParet)
        : QObject(pParet)
        , type(type)
        , id(id)
        , name(name)
    {
    }

    const PlayerType type;
    const FBBPlayerId id;
    const QString name;

    FBBTeam team = FBBTeam::Unknown;
    uint32_t age = 0;
    uint32_t experience = 0;


    struct Appearances
    {
        Appearances()
        {
            memset(this, 0, sizeof(Appearances));
        }

        struct Fielding
        {
            float C;
            float _1B;
            float _2B;
            float SS;
            float _3B;
            float LF;
            float CF;
            float RF;
        } fielding;
    } appearances;

    struct Projection
    {
        struct Hitting
        {
            uint32_t PA  = 0;
            uint32_t AB  = 0;
            uint32_t H   = 0;
            uint32_t _2B = 0;
            uint32_t _3B = 0;
            uint32_t HR  = 0;
            uint32_t R   = 0;
            uint32_t RBI = 0;
            uint32_t BB  = 0;
            uint32_t SO  = 0;
            uint32_t HBP = 0;
            uint32_t SB  = 0;
            uint32_t CS  = 0;

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
            uint32_t W  = 0;
            uint32_t L  = 0;
            uint32_t GS = 0;
            uint32_t G  = 0;
            uint32_t SV = 0;
            uint32_t IP = 0;
            uint32_t H  = 0;
            uint32_t ER = 0;
            uint32_t HR = 0;
            uint32_t SO = 0;
            uint32_t BB = 0;

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

    FBBPositionMask EligablePositions() const
    {
        const float inningEligibitlity = (FBBLeaugeSettings::Instance().leauge.positionEligibility * 6.f);

        FBBPositionMask mask = FBB_POSITION_UNKNOWN;

        if (appearances.fielding.C >= inningEligibitlity)
        {
            mask |= FBB_POSITION_C;
        }

        if (appearances.fielding._1B >= inningEligibitlity)
        {
            mask |= FBB_POSITION_1B;
        }

        if (appearances.fielding._2B >= inningEligibitlity)
        {
            mask |= FBB_POSITION_2B;
        }

        if (appearances.fielding.SS >= inningEligibitlity)
        {
            mask |= FBB_POSITION_SS;
        }

        if (appearances.fielding._3B >= inningEligibitlity)
        {
            mask |= FBB_POSITION_3B;
        }

        if (appearances.fielding.LF >= inningEligibitlity)
        {
            mask |= FBB_POSITION_LF;
        }

        if (appearances.fielding.CF >= inningEligibitlity)
        {
            mask |= FBB_POSITION_CF;
        }

        if (appearances.fielding.RF >= inningEligibitlity)
        {
            mask |= FBB_POSITION_RF;
        }

        return mask;
    }

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