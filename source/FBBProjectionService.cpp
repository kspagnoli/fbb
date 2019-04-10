#include "FBB/FBBProjectionService.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"
#include "FBB/FBBPlayerDataService.h"

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

void FBBProjectionService::LoadHittingProjections(const QString& file)
{
    // Log
    // GlobalLogger::AppendMessage("Loading hitting projections...");

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
        FBBPlayerDataService::AddPlayer(pPlayer);
    }
}

void FBBProjectionService::LoadPitchingProjections(const QString& file)
{
    // Log
    // GlobalLogger::AppendMessage("Loading hitting projections...");

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

        // Add to list
        FBBPlayerDataService::AddPlayer(pPlayer);
    }
}

static void LoadFielding(const QString& file)
{
    // Open file
    QFile inputFile(":/data/2019-appearances.csv");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream textStream(&inputFile);

    // Tokenize header data
    QStringList parsed = textStream.readLine().split(",");
    parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

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

        FBBPlayer* pPlayer = FBBPlayerDataService::GetPlayer(id);
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

static void CalculateHittingZScores()
{
    std::vector<FBBPlayer*> vecHitters = FBBPlayerDataService::GetValidHitters();

    struct PerHitting
    {
        double PA = 0;
        double AB = 0;
        double H = 0;
        double _2B = 0;
        double _3B = 0;
        double HR = 0;
        double R = 0;
        double RBI = 0;
        double BB = 0;
        double SO = 0;
        double HBP = 0;
        double SB = 0;
        double CS = 0;
        double AVG = 0;
    };

    PerHitting sumHitting;
    for (FBBPlayer* pHitter : vecHitters) {
        sumHitting.PA += pHitter->projection.hitting.PA;
        sumHitting.AB += pHitter->projection.hitting.AB;
        sumHitting.H += pHitter->projection.hitting.H;
        sumHitting._2B += pHitter->projection.hitting._2B;
        sumHitting._3B += pHitter->projection.hitting._3B;
        sumHitting.HR += pHitter->projection.hitting.HR;
        sumHitting.R += pHitter->projection.hitting.R;
        sumHitting.RBI += pHitter->projection.hitting.RBI;
        sumHitting.BB += pHitter->projection.hitting.BB;
        sumHitting.SO += pHitter->projection.hitting.SO;
        sumHitting.HBP += pHitter->projection.hitting.HBP;
        sumHitting.SB += pHitter->projection.hitting.SB;
        sumHitting.CS += pHitter->projection.hitting.CS;
    }

    PerHitting avgHitting;
    avgHitting.PA = sumHitting.PA / vecHitters.size();
    avgHitting.AB = sumHitting.AB / vecHitters.size();
    avgHitting.H = sumHitting.H / vecHitters.size();
    avgHitting._2B = sumHitting._2B / vecHitters.size();
    avgHitting._3B = sumHitting._3B / vecHitters.size();
    avgHitting.HR = sumHitting.HR / vecHitters.size();
    avgHitting.R = sumHitting.R / vecHitters.size();
    avgHitting.RBI = sumHitting.RBI / vecHitters.size();
    avgHitting.BB = sumHitting.BB / vecHitters.size();
    avgHitting.SO = sumHitting.SO / vecHitters.size();
    avgHitting.HBP = sumHitting.HBP / vecHitters.size();
    avgHitting.SB = sumHitting.SB / vecHitters.size();
    avgHitting.CS = sumHitting.CS / vecHitters.size();
    avgHitting.AVG = sumHitting.H / sumHitting.AB;

    PerHitting stddevHitting;
    for (FBBPlayer* pHitter : vecHitters) {
        stddevHitting.PA += std::pow(pHitter->projection.hitting.PA - avgHitting.PA, 2.0);
        stddevHitting.AB += std::pow(pHitter->projection.hitting.AB - avgHitting.AB, 2.0);
        stddevHitting.H += std::pow(pHitter->projection.hitting.H - avgHitting.H, 2.0);
        stddevHitting._2B += std::pow(pHitter->projection.hitting._2B - avgHitting._2B, 2.0);
        stddevHitting._3B += std::pow(pHitter->projection.hitting._3B - avgHitting._3B, 2.0);
        stddevHitting.HR += std::pow(pHitter->projection.hitting.HR - avgHitting.HR, 2.0);
        stddevHitting.R += std::pow(pHitter->projection.hitting.R - avgHitting.R, 2.0);
        stddevHitting.RBI += std::pow(pHitter->projection.hitting.RBI - avgHitting.RBI, 2.0);
        stddevHitting.BB += std::pow(pHitter->projection.hitting.BB - avgHitting.BB, 2.0);
        stddevHitting.SO += std::pow(pHitter->projection.hitting.SO - avgHitting.SO, 2.0);
        stddevHitting.HBP += std::pow(pHitter->projection.hitting.HBP - avgHitting.HBP, 2.0);
        stddevHitting.SB += std::pow(pHitter->projection.hitting.SB - avgHitting.SB, 2.0);
        stddevHitting.CS += std::pow(pHitter->projection.hitting.CS - avgHitting.CS, 2.0);
        stddevHitting.AVG += std::pow(pHitter->projection.hitting.AVG() - avgHitting.AVG, 2.0);
    }

    stddevHitting.PA = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.PA);
    stddevHitting.AB = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.AB);
    stddevHitting.H = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.H);
    stddevHitting._2B = std::sqrt(1. / double(vecHitters.size()) * stddevHitting._2B);
    stddevHitting._3B = std::sqrt(1. / double(vecHitters.size()) * stddevHitting._3B);
    stddevHitting.HR = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.HR);
    stddevHitting.R = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.R);
    stddevHitting.RBI = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.RBI);
    stddevHitting.BB = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.BB);
    stddevHitting.SO = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.SO);
    stddevHitting.HBP = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.HBP);
    stddevHitting.SB = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.SB);
    stddevHitting.CS = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.CS);
    stddevHitting.AVG = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.AVG);

    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.zHitting.HR = (pHitter->projection.hitting.HR - avgHitting.HR) / stddevHitting.HR;
        pHitter->calculations.zHitting.R = (pHitter->projection.hitting.R - avgHitting.R) / stddevHitting.R;
        pHitter->calculations.zHitting.RBI = (pHitter->projection.hitting.RBI - avgHitting.RBI) / stddevHitting.RBI;
        pHitter->calculations.zHitting.SB = (pHitter->projection.hitting.SB - avgHitting.SB) / stddevHitting.SB;
        pHitter->calculations.zHitting.AVG = (pHitter->projection.hitting.AVG() - avgHitting.AVG) / stddevHitting.AVG;
    }

    sumHitting.AVG = 0;
    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.zHitting.AVG *= pHitter->projection.hitting.AB;
        sumHitting.AVG += pHitter->calculations.zHitting.AVG;
    }

    stddevHitting.AVG = 0;
    for (FBBPlayer* pHitter : vecHitters) {
        stddevHitting.AVG += std::pow(pHitter->calculations.zHitting.AVG - avgHitting.AVG, 2.0);
    }
    stddevHitting.AVG = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.AVG);

    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.zHitting.AVG = (pHitter->calculations.zHitting.AVG - avgHitting.AVG) / stddevHitting.AVG;
    }

    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.zScore = 0;
        if (FBBLeaugeSettings::Instance().categories.hitting.HR) {
            pHitter->calculations.zScore += pHitter->calculations.zHitting.HR;
        }
        if (FBBLeaugeSettings::Instance().categories.hitting.R) {
            pHitter->calculations.zScore += pHitter->calculations.zHitting.R;
        }
        if (FBBLeaugeSettings::Instance().categories.hitting.RBI) {
            pHitter->calculations.zScore += pHitter->calculations.zHitting.RBI;
        }
        if (FBBLeaugeSettings::Instance().categories.hitting.SB) {
            pHitter->calculations.zScore += pHitter->calculations.zHitting.SB;
        }
        if (FBBLeaugeSettings::Instance().categories.hitting.AVG) {
            pHitter->calculations.zScore += pHitter->calculations.zHitting.AVG;
        }
    }

    // Sort by zScore
    std::sort(vecHitters.begin(), vecHitters.end(), [](const FBBPlayer* pLHS, const FBBPlayer* pRHS) {
        return pLHS->calculations.zScore > pRHS->calculations.zScore;
    });

    // Get replacement player
    const size_t numDraftedHitters = FBBLeaugeSettings::Instance().SumHitters() * FBBLeaugeSettings::Instance().owners.size();
    if (numDraftedHitters >= vecHitters.size()) {
        return;
    }
    double replacementZ = vecHitters[numDraftedHitters]->calculations.zScore;
    
    // Normalize zScores
    double sumZ = 0;
    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.zScore -= replacementZ;
        if (pHitter->calculations.zScore > 0.0) {
            sumZ += pHitter->calculations.zScore;
        }
    }

    // Calculate cost estimates
    const double totalMoney = FBBLeaugeSettings::Instance().leauge.budget * FBBLeaugeSettings::Instance().owners.size();
    const double totalHittingMoney = FBBLeaugeSettings::Instance().projections.hittingPitchingSplit * totalMoney;
    const double costPerZ = totalHittingMoney / sumZ;
    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.estimate = pHitter->calculations.zScore * costPerZ;
    }
}

static void CalculatePitchingZScores()
{
    std::vector<FBBPlayer*> vecPitchers = FBBPlayerDataService::GetValidPitchers();

    struct PerPitching
    {
        double W = 0;
        double L = 0;
        double GS = 0;
        double G = 0;
        double SV = 0;
        double IP = 0;
        double H = 0;
        double ER = 0;
        double HR = 0;
        double SO = 0;
        double BB = 0;
        double ERA = 0;
        double WHIP = 0;
    };

    PerPitching sumPitching;
    for (FBBPlayer* pPitcher : vecPitchers) {
        sumPitching.W += pPitcher->projection.pitching.W;
        sumPitching.L += pPitcher->projection.pitching.L;
        sumPitching.GS += pPitcher->projection.pitching.GS;
        sumPitching.G += pPitcher->projection.pitching.G;
        sumPitching.SV += pPitcher->projection.pitching.SV;
        sumPitching.IP += pPitcher->projection.pitching.IP;
        sumPitching.H += pPitcher->projection.pitching.H;
        sumPitching.ER += pPitcher->projection.pitching.ER;
        sumPitching.HR += pPitcher->projection.pitching.HR;
        sumPitching.SO += pPitcher->projection.pitching.SO;
        sumPitching.BB += pPitcher->projection.pitching.BB;
    }

    PerPitching avgPitching;
    avgPitching.W = sumPitching.W / vecPitchers.size();
    avgPitching.L = sumPitching.L / vecPitchers.size();
    avgPitching.GS = sumPitching.GS / vecPitchers.size();
    avgPitching.G = sumPitching.G / vecPitchers.size();
    avgPitching.SV = sumPitching.SV / vecPitchers.size();
    avgPitching.IP = sumPitching.IP / vecPitchers.size();
    avgPitching.H = sumPitching.H / vecPitchers.size();
    avgPitching.ER = sumPitching.ER / vecPitchers.size();
    avgPitching.HR = sumPitching.HR / vecPitchers.size();
    avgPitching.SO = sumPitching.SO / vecPitchers.size();
    avgPitching.BB = sumPitching.BB / vecPitchers.size();
    avgPitching.ERA = (9 * sumPitching.ER) / (sumPitching.IP);
    avgPitching.WHIP = (sumPitching.BB + sumPitching.H) / sumPitching.IP;

    PerPitching stddevPitching;
    for (FBBPlayer* pPitcher : vecPitchers) {
        stddevPitching.W += std::pow(pPitcher->projection.pitching.W - avgPitching.W, 2.0);
        stddevPitching.L += std::pow(pPitcher->projection.pitching.L - avgPitching.L, 2.0);
        stddevPitching.GS += std::pow(pPitcher->projection.pitching.GS - avgPitching.GS, 2.0);
        stddevPitching.G += std::pow(pPitcher->projection.pitching.G - avgPitching.G, 2.0);
        stddevPitching.SV += std::pow(pPitcher->projection.pitching.SV - avgPitching.SV, 2.0);
        stddevPitching.IP += std::pow(pPitcher->projection.pitching.IP - avgPitching.IP, 2.0);
        stddevPitching.H += std::pow(pPitcher->projection.pitching.H - avgPitching.H, 2.0);
        stddevPitching.ER += std::pow(pPitcher->projection.pitching.ER - avgPitching.ER, 2.0);
        stddevPitching.HR += std::pow(pPitcher->projection.pitching.HR - avgPitching.HR, 2.0);
        stddevPitching.SO += std::pow(pPitcher->projection.pitching.SO - avgPitching.SO, 2.0);
        stddevPitching.BB += std::pow(pPitcher->projection.pitching.BB - avgPitching.BB, 2.0);
        stddevPitching.ERA += std::pow(pPitcher->projection.pitching.ERA() - avgPitching.ERA, 2.0);
        stddevPitching.WHIP += std::pow(pPitcher->projection.pitching.WHIP() - avgPitching.WHIP, 2.0);
    }

    stddevPitching.W = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.W);
    stddevPitching.L = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.L);
    stddevPitching.GS = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.GS);
    stddevPitching.G = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.G);
    stddevPitching.SV = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.SV);
    stddevPitching.IP = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.IP);
    stddevPitching.H = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.H);
    stddevPitching.ER = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.ER);
    stddevPitching.HR = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.HR);
    stddevPitching.SO = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.SO);
    stddevPitching.BB = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.BB);
    stddevPitching.ERA = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.ERA);
    stddevPitching.WHIP = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.WHIP);

    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.zPitching.W = (pPitcher->projection.pitching.W - avgPitching.W) / stddevPitching.W;
        pPitcher->calculations.zPitching.SV = (pPitcher->projection.pitching.SV - avgPitching.SV) / stddevPitching.SV;
        pPitcher->calculations.zPitching.SO = (pPitcher->projection.pitching.SO - avgPitching.SO) / stddevPitching.SO;
        pPitcher->calculations.zPitching.ERA = (pPitcher->projection.pitching.ERA() - avgPitching.ERA) / stddevPitching.ERA;
        pPitcher->calculations.zPitching.WHIP = (pPitcher->projection.pitching.WHIP() - avgPitching.WHIP) / stddevPitching.WHIP;
    }

    sumPitching.ERA = 0;
    sumPitching.WHIP = 0;
    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.zPitching.ERA *= pPitcher->projection.pitching.IP;
        pPitcher->calculations.zPitching.WHIP *= pPitcher->projection.pitching.IP;
        sumPitching.ERA += pPitcher->calculations.zPitching.ERA;
        sumPitching.WHIP += pPitcher->calculations.zPitching.WHIP;
    }

    stddevPitching.ERA = 0;
    stddevPitching.WHIP = 0;
    for (FBBPlayer* pPitcher : vecPitchers) {
        stddevPitching.ERA += std::pow(pPitcher->calculations.zPitching.ERA - avgPitching.ERA, 2.0);
        stddevPitching.WHIP += std::pow(pPitcher->calculations.zPitching.WHIP - avgPitching.WHIP, 2.0);
    }
    stddevPitching.ERA = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.ERA);
    stddevPitching.WHIP = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.WHIP);

    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.zPitching.ERA = -1. * (pPitcher->calculations.zPitching.ERA - avgPitching.ERA) / stddevPitching.ERA;
        pPitcher->calculations.zPitching.WHIP = -1. * (pPitcher->calculations.zPitching.WHIP - avgPitching.WHIP) / stddevPitching.WHIP;
    }

    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.zScore = 0;
        if (FBBLeaugeSettings::Instance().categories.pitching.ERA) {
            pPitcher->calculations.zScore += pPitcher->calculations.zPitching.ERA;
        }
        if (FBBLeaugeSettings::Instance().categories.pitching.SO) {
            pPitcher->calculations.zScore += pPitcher->calculations.zPitching.SO;
        }
        if (FBBLeaugeSettings::Instance().categories.pitching.SV) {
            pPitcher->calculations.zScore += pPitcher->calculations.zPitching.SV;
        }
        if (FBBLeaugeSettings::Instance().categories.pitching.W) {
            pPitcher->calculations.zScore += pPitcher->calculations.zPitching.W;
        }
        if (FBBLeaugeSettings::Instance().categories.pitching.WHIP) {
            pPitcher->calculations.zScore += pPitcher->calculations.zPitching.WHIP;
        }
    }

    // Sort by zScore
    std::sort(vecPitchers.begin(), vecPitchers.end(), [](const FBBPlayer* pLHS, const FBBPlayer* pRHS) {
        return pLHS->calculations.zScore > pRHS->calculations.zScore;
    });

    // Get replacement player
    const size_t numDraftedPitchers = FBBLeaugeSettings::Instance().SumPitchers() * FBBLeaugeSettings::Instance().owners.size();
    if (numDraftedPitchers >= vecPitchers.size()) {
        return;
    }
    double replacementZ = vecPitchers[numDraftedPitchers]->calculations.zScore;

    // Normalize zScores
    double sumZ = 0;
    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.zScore -= replacementZ;
        if (pPitcher->calculations.zScore > 0.0) {
            sumZ += pPitcher->calculations.zScore;
        }
    }

    // Calculate cost estimates
    const double totalMoney = FBBLeaugeSettings::Instance().leauge.budget * FBBLeaugeSettings::Instance().owners.size();
    const double totalPitchingMoney = (1.0 - FBBLeaugeSettings::Instance().projections.hittingPitchingSplit) * totalMoney;
    const double costPerZ = totalPitchingMoney / sumZ;
    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.estimate = pPitcher->calculations.zScore * costPerZ;
    }
}

FBBProjectionService::FBBProjectionService(QObject* parent)
    : QObject(parent)
{
    // Listen for settings changes
    connect(&FBBLeaugeSettings::Instance(), &FBBLeaugeSettings::SettingsChanged, this, [=](const FBBLeaugeSettings& settings) {
        UpdateCalculations();
    });

    // Load initial projections
    LoadProjections(FBBLeaugeSettings::Instance().projections.source);
}

FBBProjectionService& FBBProjectionService::Instance()
{
    static FBBProjectionService* s_service = new FBBProjectionService(qApp);
    return *s_service;
}

void FBBProjectionService::LoadProjections(const FBBLeaugeSettings::Projections::Source& source)
{
    const QString hittingFile = ":/data/2019-hitters-fan.csv";
    const QString pitchingFile = ":/data/2019-pitchers-fan.csv";
    const QString appearanceFile = ":/data/2019-appearances.csv";

    FBBProjectionService::LoadHittingProjections(hittingFile);
    FBBProjectionService::LoadPitchingProjections(pitchingFile);
    LoadFielding(pitchingFile);

    UpdateCalculations();
}

void FBBProjectionService::UpdateCalculations()
{
    emit BeginProjectionsUpdated();

    CalculateHittingZScores();
    CalculatePitchingZScores();
    FBBPlayerDataService::Finalize();

    emit EndProjectionsUpdated();
}