#include "FBB/FBBPlayerDataService.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

#include <algorithm>

static FBBTeam ToFBBTeam(const QString& teamName)
{
    static const QMap<QString, FBBTeam> s_LUT = 
    {
        {"Angels", FBBTeam::LAA },
        {"Astros", FBBTeam::HOU},
        {"Athletics", FBBTeam::OAK},
        {"Blue Jays", FBBTeam::TOR},
        {"Braves", FBBTeam::ATL},
        {"Brewers", FBBTeam::MIL},
        {"Cardinals", FBBTeam::STL},
        {"Cubs", FBBTeam::CHC},
        {"Diamondbacks", FBBTeam::ARI},
        {"Dodgers", FBBTeam::LAD},
        {"Giants", FBBTeam::SF},
        {"Indians", FBBTeam::CLE},
        {"Mariners", FBBTeam::SEA},
        {"Marlins", FBBTeam::MIA},
        {"Mets", FBBTeam::NYM},
        {"Nationals", FBBTeam::WAS},
        {"Orioles", FBBTeam::BAL},
        {"Padres", FBBTeam::SD},
        {"Phillies", FBBTeam::PHI},
        {"Pirates", FBBTeam::PIT},
        {"Rangers", FBBTeam::TEX},
        {"Rays", FBBTeam::TB},
        {"Red Sox", FBBTeam::BOS},
        {"Reds", FBBTeam::CIN},
        {"Rockies", FBBTeam::COL},
        {"Royals", FBBTeam::KC},
        {"Tigers", FBBTeam::DET},
        {"Twins", FBBTeam::MIN},
        {"White Sox", FBBTeam::CHW},
        {"Yankees", FBBTeam::NYY},
    };

    // Assume no team is a free agent
    if (teamName.isEmpty()) {
        return FBBTeam::FA;
    }

    // Lookup from string
    auto itr = s_LUT.find(teamName);
    if (itr == s_LUT.end()) {
        return FBBTeam::FA;
    }
    return itr.value();
}

FBBPlayerDataService::FBBPlayerDataService(QObject* parent)
    : QObject(parent)
{
}

FBBPlayerDataService& FBBPlayerDataService::Instance()
{
    static FBBPlayerDataService* s_service = new FBBPlayerDataService(qApp);
    return *s_service;
}

uint32_t FBBPlayerDataService::PlayerCount()
{
    return Instance().m_flatData.size();
}

FBBPlayer* FBBPlayerDataService::GetPlayer(uint32_t index)
{
    if (index >= PlayerCount()) {
        return nullptr;
    }

    return Instance().m_flatData[index].get();
}

FBBPlayer* FBBPlayerDataService::GetPlayer(const FBBPlayerId& playerId)
{
    struct Compare
    {
        bool operator()(const FBBPlayerId& lhs, const std::shared_ptr<FBBPlayer>& rhs)
        {
            return lhs < rhs->id;
        }

        bool operator()(const std::shared_ptr<FBBPlayer>& lhs, const FBBPlayerId& rhs)
        {
            return lhs->id < rhs;
        }
    };

    // Lookup player
    auto itr = Instance().m_mappedData.find(playerId);
    if (itr != Instance().m_mappedData.end()) {
        return itr.value().get();
    } 

    // Create new player
    std::shared_ptr<FBBPlayer> spNewPlayer = std::make_shared<FBBPlayer>();

    // Set id
    spNewPlayer->id = playerId;

    // Add to be maps
    Instance().m_mappedData[playerId] = spNewPlayer;
    Instance().m_flatData.push_back(spNewPlayer);

    // Return the new player
    return spNewPlayer.get();
}

FBBPlayer* FBBPlayerDataService::GetPlayerFromBaseballReference(const QString& name, const QString& team)
{
    QStringList splitName = name.split("\\");

    QList<std::shared_ptr<FBBPlayer>> potentialMatches;

    // Loop all names
    for (const std::shared_ptr<FBBPlayer>& spPlayer : Instance().m_flatData) {

        // BB reference uses 2first5last2count notion
        const uint32_t count = splitName[1].right(2).toUInt();
    
        // Handle multiple name instances
        if (spPlayer->name == splitName[0]) {
            if (count == 1) {
                return spPlayer.get();
            } else {
                potentialMatches.append(spPlayer);
            }
        } 
    }

    // Only one player we know about with this name
    if (potentialMatches.size() == 1) {
        return potentialMatches[0].get();
    }

    // Look for a team match
    for (auto& spPlayer : potentialMatches) {
        if (FBBTeamToString(spPlayer->team) == team) {
            return spPlayer.get();
        }
    }

    return nullptr;
}

bool FBBPlayerDataService::IsValidUnderCurrentSettings(const FBBPlayer* pPlayer)
{
    // Has min AB
    if (pPlayer->spProjection && pPlayer->spProjection->type == FBBPlayer::Projection::PROJECTION_TYPE_HITTING) {
        if (pPlayer->spProjection->hitting.AB < FBBLeaugeSettings::Instance().projections.minAB) {
            return false;
        }
    }

    // Has min IP
    if (pPlayer->spProjection && pPlayer->spProjection->type == FBBPlayer::Projection::PROJECTION_TYPE_PITCHING) {
        if (pPlayer->spProjection->pitching.IP < FBBLeaugeSettings::Instance().projections.minIP) {
            return false;
        }
    }

    // Ignore no-AL in AL only
    if (FBBLeaugeSettings::Instance().leauge.type == FBBLeaugeSettings::Leauge::Type::AL && GetDivision(pPlayer->team) != FBBLeauge::AL) {
        return false;
    }

    // Ignore non-NL in NL only
    if (FBBLeaugeSettings::Instance().leauge.type == FBBLeaugeSettings::Leauge::Type::NL && GetDivision(pPlayer->team) != FBBLeauge::NL) {
        return false;
    }

    // Ignore free agents
    if (!FBBLeaugeSettings::Instance().projections.includeFA && GetDivision(pPlayer->team) == FBBLeauge::None) {
        return false;
    }

    // Match
    return true;
}

void FBBPlayerDataService::ForEach(const std::function<void(FBBPlayer&)>&& fn)
{
    for (const std::shared_ptr<FBBPlayer>& spPlayer : Instance().m_flatData) {
        fn(*spPlayer);
    }
}

std::vector<FBBPlayer*> FBBPlayerDataService::GetValidHitters()
{
    std::vector<FBBPlayer*> ret;
    for (const std::shared_ptr<FBBPlayer>& spPlayer : Instance().m_flatData) {
        if (spPlayer->spProjection && spPlayer->spProjection->type == FBBPlayer::Projection::PROJECTION_TYPE_HITTING) {
            if (IsValidUnderCurrentSettings(spPlayer.get())) {
                ret.push_back(spPlayer.get());
            }
        }
    }
    return ret;
}

std::vector<FBBPlayer*> FBBPlayerDataService::GetValidPitchers()
{
    std::vector<FBBPlayer*> ret;
    for (const std::shared_ptr<FBBPlayer>& spPlayer : Instance().m_flatData) {
        if (spPlayer->spProjection && spPlayer->spProjection->type == FBBPlayer::Projection::PROJECTION_TYPE_PITCHING) {
            if (IsValidUnderCurrentSettings(spPlayer.get())) {
                ret.push_back(spPlayer.get());
            }
        }
    }
    return ret;
}
