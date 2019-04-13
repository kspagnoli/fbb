#include "FBB/FBBProjectionService.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"
#include "FBB/FBBApplication.h"
#include "FBB/FBBDraftBoardModel.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

#include <memory>

static FBBTeam ToFBBTeam(const QString& teamName)
{
    static const QMap<QString, FBBTeam> s_LUT =
    {
        { "Angels", FBBTeam::LAA },
        { "Astros", FBBTeam::HOU },
        { "Athletics", FBBTeam::OAK },
        { "Blue Jays", FBBTeam::TOR },
        { "Braves", FBBTeam::ATL },
        { "Brewers", FBBTeam::MIL },
        { "Cardinals", FBBTeam::STL },
        { "Cubs", FBBTeam::CHC },
        { "Diamondbacks", FBBTeam::ARI },
        { "Dodgers", FBBTeam::LAD },
        { "Giants", FBBTeam::SF },
        { "Indians", FBBTeam::CLE },
        { "Mariners", FBBTeam::SEA },
        { "Marlins", FBBTeam::MIA },
        { "Mets", FBBTeam::NYM },
        { "Nationals", FBBTeam::WAS },
        { "Orioles", FBBTeam::BAL },
        { "Padres", FBBTeam::SD },
        { "Phillies", FBBTeam::PHI },
        { "Pirates", FBBTeam::PIT },
        { "Rangers", FBBTeam::TEX },
        { "Rays", FBBTeam::TB },
        { "Red Sox", FBBTeam::BOS },
        { "Reds", FBBTeam::CIN },
        { "Rockies", FBBTeam::COL },
        { "Royals", FBBTeam::KC },
        { "Tigers", FBBTeam::DET },
        { "Twins", FBBTeam::MIN },
        { "White Sox", FBBTeam::CHW },
        { "Yankees", FBBTeam::NYY },
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

void FBBProjectionService::LoadHittingProjections(std::vector<FBBPlayer*>& vecPlayers, const QString& file)
{
    // Open file
    QFile inputFile(file);
    inputFile.open(QIODevice::ReadOnly);
    QTextStream textStream(&inputFile);

    // Tokenize header data
    QStringList parsed = textStream.readLine().split(",");
    parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

    // Stats to find
    // Stats to find
    enum FanHitter
    {
        FAN_HITTER_NAME,
        FAN_HITTER_TEAM,
        FAN_HITTER_G,
        FAN_HITTER_PA,
        FAN_HITTER_AB,
        FAN_HITTER_H,
        FAN_HITTER_2B,
        FAN_HITTER_3B,
        FAN_HITTER_HR,
        FAN_HITTER_R,
        FAN_HITTER_RBI,
        FAN_HITTER_BB,
        FAN_HITTER_SO,
        FAN_HITTER_HBP,
        FAN_HITTER_SB,
        FAN_HITTER_CS,
        FAN_HITTER_AVG,
        FAN_HITTER_OBP,
        FAN_HITTER_SLG,
        FAN_HITTER_OPS,
        FAN_HITTER_WOBA,
        FAN_HITTER_FLD,
        FAN_HITTER_BSR,
        FAN_HITTER_WAR,
        FAN_HITTER_ADP,
        FAN_HITTER_PLAYER_ID,
    };

    // Rk,Name,Age,PA,AB,R,H,2B,3B,HR,RBI,SB,CS,BB,SO,BA,OBP,SLG,OPS,TB,GDP,HBP,SH,SF,IBB,Rel
    // 1,Jose Abreu\abreujo02,32,544,492,65,136,31,2,21,75,2,1,35,106,.276,.335,.476,.811,234,15,11,0,5,5,86%

    // Loop rows
    while (!textStream.atEnd()) {

        // Tokenize this row
        QStringList parsed = textStream.readLine().split(",");
        parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

        // Lazily get player
        FBBPlayer* pPlayer = new FBBPlayer(FBBPlayer::PLAYER_TYPE_HITTER, parsed[FAN_HITTER_PLAYER_ID], parsed[FAN_HITTER_NAME], this);
        pPlayer->team = ToFBBTeam(parsed[FAN_HITTER_TEAM]);

        // Load stats
        pPlayer->projection.hitting.PA =  parsed[FAN_HITTER_PA].toUInt();
        pPlayer->projection.hitting.AB =  parsed[FAN_HITTER_AB].toUInt();
        pPlayer->projection.hitting.H =   parsed[FAN_HITTER_H].toUInt();
        pPlayer->projection.hitting._2B = parsed[FAN_HITTER_2B].toUInt();
        pPlayer->projection.hitting._3B = parsed[FAN_HITTER_3B].toUInt();
        pPlayer->projection.hitting.HR =  parsed[FAN_HITTER_HR].toUInt();
        pPlayer->projection.hitting.R =   parsed[FAN_HITTER_R].toUInt();
        pPlayer->projection.hitting.RBI = parsed[FAN_HITTER_RBI].toUInt();
        pPlayer->projection.hitting.BB =  parsed[FAN_HITTER_BB].toUInt();
        pPlayer->projection.hitting.SO =  parsed[FAN_HITTER_SO].toUInt();
        pPlayer->projection.hitting.HBP = parsed[FAN_HITTER_HBP].toUInt();
        pPlayer->projection.hitting.SB =  parsed[FAN_HITTER_SB].toUInt();
        pPlayer->projection.hitting.CS =  parsed[FAN_HITTER_CS].toUInt();

        // Add player
        vecPlayers.push_back(pPlayer);
    }
}

void FBBProjectionService::LoadPitchingProjections(std::vector<FBBPlayer*>& vecPlayers, const QString& file)
{
    // Open file
    QFile inputFile(file);
    inputFile.open(QIODevice::ReadOnly);
    QTextStream textStream(&inputFile);

    // Tokenize header data
    QStringList parsed = textStream.readLine().split(",");
    parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

    enum FanPitcher
    {
        FAN_PITCHER_NAME,
        FAN_PITCHER_TEAM,
        FAN_PITCHER_W,
        FAN_PITCHER_L,
        FAN_PITCHER_SV,
        FAN_PITCHER_HLD,
        FAN_PITCHER_ERA,
        FAN_PITCHER_GS,
        FAN_PITCHER_G,
        FAN_PITCHER_IP,
        FAN_PITCHER_H,
        FAN_PITCHER_ER,
        FAN_PITCHER_HR,
        FAN_PITCHER_SO,
        FAN_PITCHER_BB,
        FAN_PITCHER_WHIP,
        FAN_PITCHER_Kp9,
        FAN_PITCHER_BBp9,
        FAN_PITCHER_FIP,
        FAN_PITCHER_WAR,
        FAN_PITCHER_ADP,
        FAN_PITCHER_PLAYER_ID,
    };

    // Loop rows
    while (!textStream.atEnd()) {

        // Tokenize this row
        QStringList parsed = textStream.readLine().split(",");
        parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

        // Lazily get player
        FBBPlayer* pPlayer = new FBBPlayer(FBBPlayer::PLAYER_TYPE_PITCHER, parsed[FAN_PITCHER_PLAYER_ID], parsed[FAN_PITCHER_NAME], this);
        pPlayer->team = ToFBBTeam(parsed[FAN_PITCHER_TEAM]);
        
        // Load stats
        pPlayer->projection.pitching.W =  parsed[FAN_PITCHER_W].toUInt();
        pPlayer->projection.pitching.L =  parsed[FAN_PITCHER_L].toUInt();
        pPlayer->projection.pitching.GS = parsed[FAN_PITCHER_GS].toUInt();
        pPlayer->projection.pitching.G =  parsed[FAN_PITCHER_G].toUInt();
        pPlayer->projection.pitching.SV = parsed[FAN_PITCHER_SV].toUInt();
        pPlayer->projection.pitching.IP = parsed[FAN_PITCHER_IP].toFloat();
        pPlayer->projection.pitching.H =  parsed[FAN_PITCHER_H].toUInt();
        pPlayer->projection.pitching.ER = parsed[FAN_PITCHER_ER].toUInt();
        pPlayer->projection.pitching.HR = parsed[FAN_PITCHER_HR].toUInt();
        pPlayer->projection.pitching.SO = parsed[FAN_PITCHER_SO].toUInt();
        pPlayer->projection.pitching.BB = parsed[FAN_PITCHER_BB].toUInt();

        // Add player
        vecPlayers.push_back(pPlayer);
    }
}

static FBBPlayer* GetPlayerFromSortedById(const std::vector<FBBPlayer*>& vecPlayers, const FBBPlayerId& playerId)
{
    // Lookup player
    auto itr = std::lower_bound(vecPlayers.begin(), vecPlayers.end(), playerId, [](auto& lhs, auto& rhs){
        return lhs->id < rhs;
    });

    if (itr == vecPlayers.end()) {
        return nullptr;
    } 
    
    if ((*itr)->id == playerId) {
        return (*itr);
    }

    return nullptr;
}

static void LoadFielding(std::vector<FBBPlayer*> vecPlayers, const QString& file)
{
    // Open file
    QFile inputFile(":/data/2019-appearances.csv");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream textStream(&inputFile);

    // Tokenize header data
    QStringList parsed = textStream.readLine().split(",");
    parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

    // Sort by ID so we can binary search
    std::sort(vecPlayers.begin(), vecPlayers.end(), [](const FBBPlayer* pLHS, const FBBPlayer* pRHS){
        return pLHS->id < pRHS->id;
    });

    enum
    {
        APPEARANCE_NAME,
        APPEARANCE_TEAM,
        APPEARANCE_POS,
        APPEARANCE_INN,
        APPEARANCE_RSZ,
        APPEARANCE_RCERA,
        APPEARANCE_RSB,
        APPEARANCE_RGDP,
        APPEARANCE_RARM,
        APPEARANCE_RGFP,
        APPEARANCE_RPM,
        APPEARANCE_RTS,
        APPEARANCE_DRS,
        APPEARANCE_BIZ,
        APPEARANCE_PLAYS,
        APPEARANCE_RZR,
        APPEARANCE_OOZ,
        APPEARANCE_FSR,
        APPEARANCE_FRM,
        APPEARANCE_ARM,
        APPEARANCE_DPR,
        APPEARANCE_RNGR,
        APPEARANCE_ERRR,
        APPEARANCE_UZR,
        APPEARANCE_UZRp150,
        APPEARANCE_DEF,
        APPEARANCE_PLAYERID,
    };

    // Loop rows
    while (!textStream.atEnd()) {

        // Tokenize this row
        QStringList parsed = textStream.readLine().split(",");
        parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

        QString id = parsed[APPEARANCE_PLAYERID];

        FBBPlayer* pPlayer = GetPlayerFromSortedById(vecPlayers, id);
        if (!pPlayer) {
            continue;
        }

        const QString pos = parsed[APPEARANCE_POS];
        const float count = parsed[APPEARANCE_INN].toFloat();

        static std::map<QString, FBBPlayer::Fielding> s_LUT = 
        {
            { "C", FBBPlayer::FIELDING_C},
            { "1B", FBBPlayer::FIELDING_1B},
            { "2B", FBBPlayer::FIELDING_2B},
            { "SS", FBBPlayer::FIELDING_SS},
            { "3B", FBBPlayer::FIELDING_3B},
            { "LF", FBBPlayer::FIELDING_LF},
            { "CF", FBBPlayer::FIELDING_CF},
            { "RF", FBBPlayer::FIELDING_RF},
        };

        auto itr = s_LUT.find(pos);
        if (itr == s_LUT.end()) {
            continue;
        }

        switch (itr->second)
        {
        case FBBPlayer::FIELDING_C:
            pPlayer->appearances.fielding.C += count;
            break;
        case FBBPlayer::FIELDING_1B:
            pPlayer->appearances.fielding._1B += count;
            break;
        case FBBPlayer::FIELDING_2B:
            pPlayer->appearances.fielding._2B += count;
            break;
        case FBBPlayer::FIELDING_SS:
            pPlayer->appearances.fielding.SS += count;
            break;
        case FBBPlayer::FIELDING_3B:
            pPlayer->appearances.fielding._3B += count;
            break;
        case FBBPlayer::FIELDING_LF:
            pPlayer->appearances.fielding.LF += count;
            break;
        case FBBPlayer::FIELDING_CF:
            pPlayer->appearances.fielding.CF += count;
            break;
        case FBBPlayer::FIELDING_RF:
            pPlayer->appearances.fielding.RF += count;
            break;
        default:
            break;
        }
    }
}

FBBProjectionService::FBBProjectionService(QObject* parent)
    : QObject(parent)
{
    LoadProjections();
}

FBBProjectionService& FBBProjectionService::Instance()
{
    static FBBProjectionService* s_service = new FBBProjectionService(qApp);
    return *s_service;
}

void FBBProjectionService::LoadProjections()
{
    const QString hittingFile = ":/data/2019-hitters-fan.csv";
    const QString pitchingFile = ":/data/2019-pitchers-fan.csv";
    const QString appearanceFile = ":/data/2019-appearances.csv";

    std::vector<FBBPlayer*> vecPlayers;
    FBBProjectionService::LoadHittingProjections(vecPlayers, hittingFile);
    FBBProjectionService::LoadPitchingProjections(vecPlayers, pitchingFile);
    LoadFielding(vecPlayers, pitchingFile);

    fbbApp->DraftBoardModel()->Reset(vecPlayers);
}
