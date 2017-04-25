#include "FBB/FBBTeam.h"

QString TeamToString(const FBBTeam& team)
{
    switch (team)
    {
    case FBBTeam::LAA:
        return "LAA";
    case FBBTeam::HOU:
        return "HOU";
    case FBBTeam::OAK:
        return "OAK";
    case FBBTeam::TOR:
        return "TOR";
    case FBBTeam::ATL:
        return "ATL";
    case FBBTeam::MIL:
        return "MIL";
    case FBBTeam::STL:
        return "STL";
    case FBBTeam::CHC:
        return "CHC";
    case FBBTeam::ARI:
        return "ARI";
    case FBBTeam::LAD:
        return "LAD";
    case FBBTeam::SF:
        return "SF";
    case FBBTeam::CLE:
        return "CLE";
    case FBBTeam::SEA:
        return "SEA";
    case FBBTeam::MIA:
        return "MIA";
    case FBBTeam::NYM:
        return "NYM";
    case FBBTeam::WAS:
        return "WAS";
    case FBBTeam::BAL:
        return "BAL";
    case FBBTeam::SD:
        return "SD";
    case FBBTeam::PHI:
        return "PHI";
    case FBBTeam::PIT:
        return "PIT";
    case FBBTeam::TEX:
        return "TEX";
    case FBBTeam::TB:
        return "TB";
    case FBBTeam::BOS:
        return "BOS";
    case FBBTeam::CIN:
        return "CIN";
    case FBBTeam::COL:
        return "COL";
    case FBBTeam::KC:
        return "KC";
    case FBBTeam::DET:
        return "DET";
    case FBBTeam::MIN:
        return "MIN";
    case FBBTeam::CHW:
        return "CHW";
    case FBBTeam::NYY:
        return "NYY";
    case FBBTeam::FA:
        return "FA";
    case FBBTeam::Unknown:
    default:
        return "Unknown";
    }
}