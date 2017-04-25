#pragma once

#include "FBB/FBBTeam.h"

#include <QString>
#include <cstdint>

using PlayerID = QString;

class FBBPlayer
{
public:

    PlayerID id;

    QString name;
    FBBTeam team = FBBTeam::Unknown;

    struct Projection
    {
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
            uint32_t ADP;

            float AVG() const
            {
                return AB > 0 ? H / float(AB) : 0.f;
            }
        } hitting;

        struct Pitching
        {
            // "Name"
            // "Team"
            uint32_t W;
            uint32_t L;
            // uint32_t ERA;
            uint32_t GS;
            uint32_t G;
            uint32_t SV;
            float IP;
            uint32_t H;
            uint32_t ER;
            uint32_t HR;
            uint32_t SO;
            uint32_t BB;
            // "WHIP"
            // "K/9"
            // "BB/9"
            // "FIP"
            // "WAR"
            // "RA9-WAR"
            uint32_t ADP;
            // "playerid"

            float ERA() const
            {
                return IP > 0 ? ER / float(IP) : 0.f;
            }

            float WHIP() const
            {
                return IP > 0 ? (BB + H) / float(IP) : 0.f;
            }
        } pitching;

    };

    QSharedPointer<Projection> spProjection;
};