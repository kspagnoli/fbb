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

static void LoadHittingProjections(const QString& file)
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
    QMap<QString, int32_t> LUT;
    LUT["Name"] = parsed.indexOf("Name");
    LUT["Team"] = parsed.indexOf("Team");
    LUT["PA"] = parsed.indexOf("PA");
    LUT["AB"] = parsed.indexOf("AB");
    LUT["H"] = parsed.indexOf("H");
    LUT["2B"] = parsed.indexOf("2B");
    LUT["3B"] = parsed.indexOf("3B");
    LUT["HR"] = parsed.indexOf("HR");
    LUT["R"] = parsed.indexOf("R");
    LUT["RBI"] = parsed.indexOf("RBI");
    LUT["BB"] = parsed.indexOf("BB");
    LUT["SO"] = parsed.indexOf("SO");
    LUT["HBP"] = parsed.indexOf("HBP");
    LUT["SB"] = parsed.indexOf("SB");
    LUT["CS"] = parsed.indexOf("CS");
    LUT["playerid"] = parsed.indexOf("playerid");

    // Loop rows
    while (!textStream.atEnd()) {

        // Tokenize this row
        QStringList parsed = textStream.readLine().split(",");
        parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

        // Lazily get player
        FBBPlayer* pPlayer = FBBPlayerDataService::GetPlayer(parsed[LUT["playerid"]]);
        pPlayer->name = parsed[LUT["Name"]];
        pPlayer->team = ToFBBTeam(parsed[LUT["Team"]]);
        pPlayer->id = parsed[LUT["playerid"]];

        // Create projections
        std::unique_ptr<FBBPlayer::Projection> spProjection = std::make_unique<FBBPlayer::Projection>();

        // Load stats
        spProjection->type = FBBPlayer::Projection::PROJECTION_TYPE_HITTING;
        spProjection->hitting.PA = parsed[LUT["PA"]].toUInt();
        spProjection->hitting.AB = parsed[LUT["AB"]].toUInt();
        spProjection->hitting.H = parsed[LUT["H"]].toUInt();
        spProjection->hitting._2B = parsed[LUT["2B"]].toUInt();
        spProjection->hitting._3B = parsed[LUT["3B"]].toUInt();
        spProjection->hitting.HR = parsed[LUT["HR"]].toUInt();
        spProjection->hitting.R = parsed[LUT["R"]].toUInt();
        spProjection->hitting.RBI = parsed[LUT["RBI"]].toUInt();
        spProjection->hitting.BB = parsed[LUT["BB"]].toUInt();
        spProjection->hitting.SO = parsed[LUT["SO"]].toUInt();
        spProjection->hitting.HBP = parsed[LUT["HBP"]].toUInt();
        spProjection->hitting.SB = parsed[LUT["SB"]].toUInt();
        spProjection->hitting.CS = parsed[LUT["CS"]].toUInt();

        // Save to player
        pPlayer->spProjection = std::move(spProjection);
    }
}

static void LoadPitchingProjections(const QString& file)
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
    QMap<QString, int32_t> LUT;
    LUT["Name"] = parsed.indexOf("Name");
    LUT["Team"] = parsed.indexOf("Team");
    LUT["W"] = parsed.indexOf("W");
    LUT["L"] = parsed.indexOf("L");
    LUT["GS"] = parsed.indexOf("GS");
    LUT["G"] = parsed.indexOf("G");
    LUT["SV"] = parsed.indexOf("SV");
    LUT["IP"] = parsed.indexOf("IP");
    LUT["H"] = parsed.indexOf("H");
    LUT["ER"] = parsed.indexOf("ER");
    LUT["HR"] = parsed.indexOf("HR");
    LUT["SO"] = parsed.indexOf("SO");
    LUT["BB"] = parsed.indexOf("BB");
    LUT["ADP"] = parsed.indexOf("ADP");
    LUT["playerid"] = parsed.indexOf("playerid");

    // Loop rows
    while (!textStream.atEnd()) {

        // Tokenize this row
        QStringList parsed = textStream.readLine().split(",");
        parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

        // Lazily get player
        FBBPlayer* pPlayer = FBBPlayerDataService::GetPlayer(parsed[LUT["playerid"]]);
        pPlayer->name = parsed[LUT["Name"]];
        pPlayer->team = ToFBBTeam(parsed[LUT["Team"]]);
        
        // Create projections
        std::unique_ptr<FBBPlayer::Projection> spProjection = std::make_unique<FBBPlayer::Projection>();

        // Load stats
        spProjection->type = FBBPlayer::Projection::PROJECTION_TYPE_PITCHING;
        spProjection->pitching.W = parsed[LUT["W"]].toUInt();
        spProjection->pitching.L = parsed[LUT["L"]].toUInt();
        spProjection->pitching.GS = parsed[LUT["GS"]].toUInt();
        spProjection->pitching.G = parsed[LUT["G"]].toUInt();
        spProjection->pitching.SV = parsed[LUT["SV"]].toUInt();
        spProjection->pitching.IP = parsed[LUT["IP"]].toFloat();
        spProjection->pitching.H = parsed[LUT["H"]].toUInt();
        spProjection->pitching.ER = parsed[LUT["ER"]].toUInt();
        spProjection->pitching.HR = parsed[LUT["HR"]].toUInt();
        spProjection->pitching.SO = parsed[LUT["SO"]].toUInt();
        spProjection->pitching.BB = parsed[LUT["BB"]].toUInt();

        // Save to player
        pPlayer->spProjection = std::move(spProjection);
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
        sumHitting.PA += pHitter->spProjection->hitting.PA;
        sumHitting.AB += pHitter->spProjection->hitting.AB;
        sumHitting.H += pHitter->spProjection->hitting.H;
        sumHitting._2B += pHitter->spProjection->hitting._2B;
        sumHitting._3B += pHitter->spProjection->hitting._3B;
        sumHitting.HR += pHitter->spProjection->hitting.HR;
        sumHitting.R += pHitter->spProjection->hitting.R;
        sumHitting.RBI += pHitter->spProjection->hitting.RBI;
        sumHitting.BB += pHitter->spProjection->hitting.BB;
        sumHitting.SO += pHitter->spProjection->hitting.SO;
        sumHitting.HBP += pHitter->spProjection->hitting.HBP;
        sumHitting.SB += pHitter->spProjection->hitting.SB;
        sumHitting.CS += pHitter->spProjection->hitting.CS;
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
        stddevHitting.PA += std::pow(pHitter->spProjection->hitting.PA - avgHitting.PA, 2.0);
        stddevHitting.AB += std::pow(pHitter->spProjection->hitting.AB - avgHitting.AB, 2.0);
        stddevHitting.H += std::pow(pHitter->spProjection->hitting.H - avgHitting.H, 2.0);
        stddevHitting._2B += std::pow(pHitter->spProjection->hitting._2B - avgHitting._2B, 2.0);
        stddevHitting._3B += std::pow(pHitter->spProjection->hitting._3B - avgHitting._3B, 2.0);
        stddevHitting.HR += std::pow(pHitter->spProjection->hitting.HR - avgHitting.HR, 2.0);
        stddevHitting.R += std::pow(pHitter->spProjection->hitting.R - avgHitting.R, 2.0);
        stddevHitting.RBI += std::pow(pHitter->spProjection->hitting.RBI - avgHitting.RBI, 2.0);
        stddevHitting.BB += std::pow(pHitter->spProjection->hitting.BB - avgHitting.BB, 2.0);
        stddevHitting.SO += std::pow(pHitter->spProjection->hitting.SO - avgHitting.SO, 2.0);
        stddevHitting.HBP += std::pow(pHitter->spProjection->hitting.HBP - avgHitting.HBP, 2.0);
        stddevHitting.SB += std::pow(pHitter->spProjection->hitting.SB - avgHitting.SB, 2.0);
        stddevHitting.CS += std::pow(pHitter->spProjection->hitting.CS - avgHitting.CS, 2.0);
        stddevHitting.AVG += std::pow(pHitter->spProjection->hitting.AVG() - avgHitting.AVG, 2.0);
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
        pHitter->calculations.zHitting.HR = (pHitter->spProjection->hitting.HR - avgHitting.HR) / stddevHitting.HR;
        pHitter->calculations.zHitting.R = (pHitter->spProjection->hitting.R - avgHitting.R) / stddevHitting.R;
        pHitter->calculations.zHitting.RBI = (pHitter->spProjection->hitting.RBI - avgHitting.RBI) / stddevHitting.RBI;
        pHitter->calculations.zHitting.SB = (pHitter->spProjection->hitting.SB - avgHitting.SB) / stddevHitting.SB;
        pHitter->calculations.zHitting.AVG = (pHitter->spProjection->hitting.AVG() - avgHitting.AVG) / stddevHitting.AVG;
    }

    sumHitting.AVG = 0;
    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.zHitting.AVG *= pHitter->spProjection->hitting.AB;
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
    const uint32_t numDraftedHitters = FBBLeaugeSettings::Instance().SumHitters() * FBBLeaugeSettings::Instance().owners.size();
    if (numDraftedHitters >= vecHitters.size()) {
        return;
    }
    double replacementZ = vecHitters[numDraftedHitters]->calculations.zScore;
    
    // Normalize zScores
    int rank = 1;
    double sumZ = 0;
    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.zScore -= replacementZ;
        pHitter->calculations.rank = rank++;
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
        sumPitching.W += pPitcher->spProjection->pitching.W;
        sumPitching.L += pPitcher->spProjection->pitching.L;
        sumPitching.GS += pPitcher->spProjection->pitching.GS;
        sumPitching.G += pPitcher->spProjection->pitching.G;
        sumPitching.SV += pPitcher->spProjection->pitching.SV;
        sumPitching.IP += pPitcher->spProjection->pitching.IP;
        sumPitching.H += pPitcher->spProjection->pitching.H;
        sumPitching.ER += pPitcher->spProjection->pitching.ER;
        sumPitching.HR += pPitcher->spProjection->pitching.HR;
        sumPitching.SO += pPitcher->spProjection->pitching.SO;
        sumPitching.BB += pPitcher->spProjection->pitching.BB;
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
        stddevPitching.W += std::pow(pPitcher->spProjection->pitching.W - avgPitching.W, 2.0);
        stddevPitching.L += std::pow(pPitcher->spProjection->pitching.L - avgPitching.L, 2.0);
        stddevPitching.GS += std::pow(pPitcher->spProjection->pitching.GS - avgPitching.GS, 2.0);
        stddevPitching.G += std::pow(pPitcher->spProjection->pitching.G - avgPitching.G, 2.0);
        stddevPitching.SV += std::pow(pPitcher->spProjection->pitching.SV - avgPitching.SV, 2.0);
        stddevPitching.IP += std::pow(pPitcher->spProjection->pitching.IP - avgPitching.IP, 2.0);
        stddevPitching.H += std::pow(pPitcher->spProjection->pitching.H - avgPitching.H, 2.0);
        stddevPitching.ER += std::pow(pPitcher->spProjection->pitching.ER - avgPitching.ER, 2.0);
        stddevPitching.HR += std::pow(pPitcher->spProjection->pitching.HR - avgPitching.HR, 2.0);
        stddevPitching.SO += std::pow(pPitcher->spProjection->pitching.SO - avgPitching.SO, 2.0);
        stddevPitching.BB += std::pow(pPitcher->spProjection->pitching.BB - avgPitching.BB, 2.0);
        stddevPitching.ERA += std::pow(pPitcher->spProjection->pitching.ERA() - avgPitching.ERA, 2.0);
        stddevPitching.WHIP += std::pow(pPitcher->spProjection->pitching.WHIP() - avgPitching.WHIP, 2.0);
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
        pPitcher->calculations.zPitching.W = (pPitcher->spProjection->pitching.W - avgPitching.W) / stddevPitching.W;
        pPitcher->calculations.zPitching.SV = (pPitcher->spProjection->pitching.SV - avgPitching.SV) / stddevPitching.SV;
        pPitcher->calculations.zPitching.SO = (pPitcher->spProjection->pitching.SO - avgPitching.SO) / stddevPitching.SO;
        pPitcher->calculations.zPitching.ERA = (pPitcher->spProjection->pitching.ERA() - avgPitching.ERA) / stddevPitching.ERA;
        pPitcher->calculations.zPitching.WHIP = (pPitcher->spProjection->pitching.WHIP() - avgPitching.WHIP) / stddevPitching.WHIP;
    }

    sumPitching.ERA = 0;
    sumPitching.WHIP = 0;
    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.zPitching.ERA *= pPitcher->spProjection->pitching.IP;
        pPitcher->calculations.zPitching.WHIP *= pPitcher->spProjection->pitching.IP;
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
    const uint32_t numDraftedPitchers = FBBLeaugeSettings::Instance().SumPitchers() * FBBLeaugeSettings::Instance().owners.size();
    if (numDraftedPitchers >= vecPitchers.size()) {
        return;
    }
    double replacementZ = vecPitchers[numDraftedPitchers]->calculations.zScore;

    // Normalize zScores
    int rank = 1;
    double sumZ = 0;
    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.zScore -= replacementZ;
        pPitcher->calculations.rank = rank++;
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
        LoadProjections(FBBLeaugeSettings::Instance().projections.source);
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
    const QString hittingFile = [&]() -> QString
    {
        switch (source)
        {
        case FBBLeaugeSettings::Projections::Source::ZiPS:
            return ":/data/2017-hitters-zips.csv";
        case FBBLeaugeSettings::Projections::Source::Fans:
            return ":/data/2017-hitters-fans.csv";
        case FBBLeaugeSettings::Projections::Source::Steamer:
            return ":/data/2017-hitters-steamer.csv";
        case FBBLeaugeSettings::Projections::Source::DepthCharts:
            return ":/data/2017-hitters-depthcharts.csv";
        case FBBLeaugeSettings::Projections::Source::ATC:
            return ":/data/2017-hitters-atc.csv";
        default:
            return QString();
        }
    }();

    const QString pitchingFile = [&]() -> QString
    {
        switch (source)
        {
        case FBBLeaugeSettings::Projections::Source::ZiPS:
            return ":/data/2017-pitchers-zips.csv";
        case FBBLeaugeSettings::Projections::Source::Fans:
            return ":/data/2017-pitchers-fans.csv";
        case FBBLeaugeSettings::Projections::Source::Steamer:
            return ":/data/2017-pitchers-steamer.csv";
        case FBBLeaugeSettings::Projections::Source::DepthCharts:
            return ":/data/2017-pitchers-depthcharts.csv";
        case FBBLeaugeSettings::Projections::Source::ATC:
            return ":/data/2017-pitchers-atc.csv";
        default:
            return QString();
        }
    }();

    emit BeginProjectionsUpdated();
    
    // Clear existing projections
    FBBPlayerDataService::ForEach([](FBBPlayer& player) {
        player.spProjection.reset();
    });

    // Load new hitting projections
    LoadHittingProjections(hittingFile);

    // Load new pitching projections
    LoadPitchingProjections(pitchingFile);

    CalculateHittingZScores();
    CalculatePitchingZScores();

    emit EndProjectionsUpdated();
}
