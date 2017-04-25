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

const QSharedPointer<FBBPlayer> FBBPlayerDataService::GetPlayer(uint32_t index)
{
    return Instance().m_flatData[index];
}

const QSharedPointer<FBBPlayer> FBBPlayerDataService::GetPlayer(const FBBPlayerID& playerId)
{
    struct Compare
    {
        bool operator()(const FBBPlayerID& lhs, const QSharedPointer<FBBPlayer>& rhs)
        {
            return lhs < rhs->id;
        }

        bool operator()(const QSharedPointer<FBBPlayer>& lhs, const FBBPlayerID& rhs)
        {
            return lhs->id < rhs;
        }
    };

    // auto itr_xx = std::binary_search(Instance().m_flatData.begin(), Instance().m_flatData.end(), playerId, Compare());

    // Lookup player
    auto itr = Instance().m_mappedData.find(playerId);
    if (itr != Instance().m_mappedData.end()) {
        return itr.value();
    } 

    // Create new player
    QSharedPointer<FBBPlayer> spNewPlayer = QSharedPointer<FBBPlayer>::create();

    // Add to be maps
    Instance().m_mappedData[playerId] = spNewPlayer;
    Instance().m_flatData.append(spNewPlayer);

    // Return the new player
    return std::move(spNewPlayer);
}

void FBBPlayerDataService::ForEach(const std::function<void(FBBPlayer&)>&& fn)
{
    for (const QSharedPointer<FBBPlayer>& spPlayer : Instance().m_flatData) {
        fn(*spPlayer);
    }
}
