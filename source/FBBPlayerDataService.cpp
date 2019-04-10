#include "FBB/FBBPlayerDataService.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"
#include "FBB/FBBDraftBoardModel.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>

#include <algorithm>

static uint32_t FindLSB(uint32_t mask)
{
    unsigned result = 0;
    while (!(mask & 1u))
    {
        mask >>= 1;
        ++result;
    }
    return result;
}

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
    // Sanitize deleted owners
    connect(&FBBLeaugeSettings::Instance(), &FBBLeaugeSettings::SettingsChanged, this, [=](const FBBLeaugeSettings& settings) {
        for (FBBPlayer* pPlayer : m_flatData) {
            if (pPlayer->draftInfo.owner) {
                auto itr = settings.owners.find(pPlayer->draftInfo.owner);
                if (itr == settings.owners.end()) {
                    pPlayer->draftInfo = FBBPlayer::DraftInfo{};
                }
            }
        }
    });
}

FBBPlayerDataService& FBBPlayerDataService::Instance()
{
    static FBBPlayerDataService* s_service = new FBBPlayerDataService(qApp);
    return *s_service;
}

uint32_t FBBPlayerDataService::PlayerCount()
{
    return (uint32_t) Instance().m_flatData.size();
}

void FBBPlayerDataService::AddPlayer(FBBPlayer* pPlayer)
{
    return Instance().m_flatData.push_back(pPlayer);
}

FBBPlayer* FBBPlayerDataService::GetPlayer(uint32_t index)
{
    if (index >= PlayerCount()) {
        return nullptr;
    }

    return Instance().m_flatData[index];
}

FBBPlayer* FBBPlayerDataService::GetPlayer(const FBBPlayerId& playerId)
{
    // Lookup player
    auto itr = std::find_if(Instance().m_flatData.begin(), Instance().m_flatData.end(), [&](const FBBPlayer* pPlayer){
        return pPlayer->id == playerId;
    });

    // TODO: binary search
    if (itr == Instance().m_flatData.end()) {
        return nullptr;
    } else {
        return *itr;
    }
}

void FBBPlayerDataService::Finalize()
{
    std::sort(Instance().m_flatData.begin(), Instance().m_flatData.end(), [](const FBBPlayer* pLHS, const FBBPlayer* pRHS){
        return pLHS->calculations.zScore > pRHS->calculations.zScore;
    });

    for (size_t i = 0; i < Instance().m_flatData.size(); i++) {
        Instance().m_flatData[i]->calculations.rank = uint32_t(i)+1;
    }

    std::sort(Instance().m_flatData.begin(), Instance().m_flatData.end(), [](const FBBPlayer* pLHS, const FBBPlayer* pRHS){
        return pLHS->id < pRHS->id;
    });
}

void FBBPlayerDataService::SetDraftModel(FBBDraftBoardModel* pModel)
{
    Instance().m_pModel = pModel;
}

bool FBBPlayerDataService::Save()
{
    QJsonObject json = Instance().m_pModel->ToJson();

    QJsonDocument doc(json);
    doc.toJson(QJsonDocument::Indented).toStdString();

    return true;
}

bool FBBPlayerDataService::Load()
{
    return true;
}

bool FBBPlayerDataService::IsValidUnderCurrentSettings(const FBBPlayer* pPlayer)
{
    // Has min AB
    if (pPlayer->type == FBBPlayer::PLAYER_TYPE_HITTER) {
        if (pPlayer->projection.hitting.AB < FBBLeaugeSettings::Instance().projections.minAB) {
            return false;
        }
    }

    // Has min IP
    if (pPlayer->type == FBBPlayer::PLAYER_TYPE_PITCHER) {
        if (pPlayer->projection.pitching.IP < FBBLeaugeSettings::Instance().projections.minIP) {
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
    for (FBBPlayer* pPlayer : Instance().m_flatData) {
        fn(*pPlayer);
    }
}

std::vector<FBBPlayer*> FBBPlayerDataService::GetValidHitters()
{
    std::vector<FBBPlayer*> ret;
    for (FBBPlayer* pPlayer : Instance().m_flatData) {
        if (pPlayer->type == FBBPlayer::PLAYER_TYPE_HITTER) {
            if (IsValidUnderCurrentSettings(pPlayer)) {
                ret.push_back(pPlayer);
            }
        }
    }
    return ret;
}

std::vector<FBBPlayer*> FBBPlayerDataService::GetValidPitchers()
{
    std::vector<FBBPlayer*> ret;
    for (FBBPlayer* pPlayer : Instance().m_flatData) {
        if (pPlayer->type == FBBPlayer::PLAYER_TYPE_PITCHER) {
            if (IsValidUnderCurrentSettings(pPlayer)) {
                ret.push_back(pPlayer);
            }
        }
    }
    return ret;
}

void FBBPlayerDataService::AddDemoData()
{
    // Get hitters
    std::vector<FBBPlayer*> vecHitters = GetValidHitters();
    std::sort(vecHitters.begin(), vecHitters.end(), [](const FBBPlayer* pLHS, const FBBPlayer* pRHS) {
        return pLHS->calculations.rank < pRHS->calculations.rank;
    });
    auto itrHitter = vecHitters.begin();

    // Loop hitter count
    for (uint32_t i = 0; i < FBBLeaugeSettings::Instance().SumHitters(); i+=4) {

        // Loop owners
        for (auto& entry : FBBLeaugeSettings::Instance().owners) {
            
            // Spoof info
            FBBPlayer* pPlayer = *itrHitter;
            pPlayer->draftInfo.owner = entry.first;
            pPlayer->draftInfo.paid = pPlayer->calculations.estimate;
            pPlayer->draftInfo.position = pPlayer->EligablePositions() == 0 ? FBB_POSITION_UNKNOWN : static_cast<FBBPositionBits>(FindLSB(pPlayer->EligablePositions()));

            // Signal
            emit Instance().PlayerDrafted(pPlayer);

            // Next
            itrHitter++;
        }
    }

    // Get pitchers
    std::vector<FBBPlayer*> vecPitchers = GetValidPitchers();
    std::sort(vecPitchers.begin(), vecPitchers.end(), [](const FBBPlayer* pLHS, const FBBPlayer* pRHS) {
        return pLHS->calculations.rank < pRHS->calculations.rank;
    });
    auto itrPitcher = vecPitchers.begin();

    // Loop hitter count
    for (uint32_t i = 0; i < FBBLeaugeSettings::Instance().SumPitchers(); i += 4) {

        // Loop owners
        for (auto& entry : FBBLeaugeSettings::Instance().owners) {

            // Spoof info
            FBBPlayer* pPlayer = *itrPitcher;
            pPlayer->draftInfo.owner = entry.first;
            pPlayer->draftInfo.paid = pPlayer->calculations.estimate;
            pPlayer->draftInfo.position = pPlayer->EligablePositions() == 0 ? FBB_POSITION_UNKNOWN : static_cast<FBBPositionBits>(FindLSB(pPlayer->EligablePositions()));

            // Signal
            emit Instance().PlayerDrafted(pPlayer);

            // Next
            itrPitcher++;
        }
    }
}
