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

static void CalculateZScores()
{
    double numHitters = 0;
    
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
    FBBPlayerDataService::ForEachValidHitter([&](const FBBPlayer& player) {
        numHitters++;
        sumHitting.PA += player.spProjection->hitting.PA;
        sumHitting.AB += player.spProjection->hitting.AB;
        sumHitting.H += player.spProjection->hitting.H;
        sumHitting._2B += player.spProjection->hitting._2B;
        sumHitting._3B += player.spProjection->hitting._3B;
        sumHitting.HR += player.spProjection->hitting.HR;
        sumHitting.R += player.spProjection->hitting.R;
        sumHitting.RBI += player.spProjection->hitting.RBI;
        sumHitting.BB += player.spProjection->hitting.BB;
        sumHitting.SO += player.spProjection->hitting.SO;
        sumHitting.HBP += player.spProjection->hitting.HBP;
        sumHitting.SB += player.spProjection->hitting.SB;
        sumHitting.CS += player.spProjection->hitting.CS;
    });

    PerHitting avgHitting;
    avgHitting.PA = sumHitting.PA / numHitters;
    avgHitting.AB = sumHitting.AB / numHitters;
    avgHitting.H = sumHitting.H / numHitters;
    avgHitting._2B = sumHitting._2B / numHitters;
    avgHitting._3B = sumHitting._3B / numHitters;
    avgHitting.HR = sumHitting.HR / numHitters;
    avgHitting.R = sumHitting.R / numHitters;
    avgHitting.RBI = sumHitting.RBI / numHitters;
    avgHitting.BB = sumHitting.BB / numHitters;
    avgHitting.SO = sumHitting.SO / numHitters;
    avgHitting.HBP = sumHitting.HBP / numHitters;
    avgHitting.SB = sumHitting.SB / numHitters;
    avgHitting.CS = sumHitting.CS / numHitters;
    avgHitting.AVG = sumHitting.H / sumHitting.AB;

    PerHitting stddevHitting;
    FBBPlayerDataService::ForEachValidHitter([&](const FBBPlayer& player) {
        stddevHitting.PA += std::pow(player.spProjection->hitting.PA - avgHitting.PA, 2.0);
        stddevHitting.AB += std::pow(player.spProjection->hitting.AB - avgHitting.AB, 2.0);
        stddevHitting.H += std::pow(player.spProjection->hitting.H - avgHitting.H, 2.0);
        stddevHitting._2B += std::pow(player.spProjection->hitting._2B - avgHitting._2B, 2.0);
        stddevHitting._3B += std::pow(player.spProjection->hitting._3B - avgHitting._3B, 2.0);
        stddevHitting.HR += std::pow(player.spProjection->hitting.HR - avgHitting.HR, 2.0);
        stddevHitting.R += std::pow(player.spProjection->hitting.R - avgHitting.R, 2.0);
        stddevHitting.RBI += std::pow(player.spProjection->hitting.RBI - avgHitting.RBI, 2.0);
        stddevHitting.BB += std::pow(player.spProjection->hitting.BB - avgHitting.BB, 2.0);
        stddevHitting.SO += std::pow(player.spProjection->hitting.SO - avgHitting.SO, 2.0);
        stddevHitting.HBP += std::pow(player.spProjection->hitting.HBP - avgHitting.HBP, 2.0);
        stddevHitting.SB += std::pow(player.spProjection->hitting.SB - avgHitting.SB, 2.0);
        stddevHitting.CS += std::pow(player.spProjection->hitting.CS - avgHitting.CS, 2.0);
        stddevHitting.AVG += std::pow(player.spProjection->hitting.AVG() - avgHitting.AVG, 2.0);
    });

    stddevHitting.PA  = std::sqrt(1./ double(numHitters) * stddevHitting.PA);
    stddevHitting.AB  = std::sqrt(1./ double(numHitters) * stddevHitting.AB);
    stddevHitting.H   = std::sqrt(1./ double(numHitters) * stddevHitting.H);
    stddevHitting._2B = std::sqrt(1./ double(numHitters) * stddevHitting._2B);
    stddevHitting._3B = std::sqrt(1./ double(numHitters) * stddevHitting._3B);
    stddevHitting.HR  = std::sqrt(1./ double(numHitters) * stddevHitting.HR);
    stddevHitting.R   = std::sqrt(1./ double(numHitters) * stddevHitting.R);
    stddevHitting.RBI = std::sqrt(1./ double(numHitters) * stddevHitting.RBI);
    stddevHitting.BB  = std::sqrt(1./ double(numHitters) * stddevHitting.BB);
    stddevHitting.SO  = std::sqrt(1./ double(numHitters) * stddevHitting.SO);
    stddevHitting.HBP = std::sqrt(1./ double(numHitters) * stddevHitting.HBP);
    stddevHitting.SB  = std::sqrt(1./ double(numHitters) * stddevHitting.SB);
    stddevHitting.CS  = std::sqrt(1./ double(numHitters) * stddevHitting.CS);
    stddevHitting.AVG = std::sqrt(1. / double(numHitters) * stddevHitting.AVG);

    FBBPlayerDataService::ForEachValidHitter([&](FBBPlayer& player) {
        player.calculations.zHitting.HR = (player.spProjection->hitting.HR - avgHitting.HR) / stddevHitting.HR;
        player.calculations.zHitting.R = (player.spProjection->hitting.R - avgHitting.R) / stddevHitting.R;
        player.calculations.zHitting.RBI = (player.spProjection->hitting.RBI - avgHitting.RBI) / stddevHitting.RBI;
        player.calculations.zHitting.SB = (player.spProjection->hitting.SB - avgHitting.SB) / stddevHitting.SB;
        player.calculations.zHitting.AVG = (player.spProjection->hitting.AVG() - avgHitting.AVG) / stddevHitting.AVG;
    });

    sumHitting.AVG = 0;
    FBBPlayerDataService::ForEachValidHitter([&](FBBPlayer& player) {
        player.calculations.zHitting.AVG *= player.spProjection->hitting.AB;
        sumHitting.AVG += player.calculations.zHitting.AVG;
    });

    stddevHitting.AVG = 0;
    FBBPlayerDataService::ForEachValidHitter([&](FBBPlayer& player) {
        stddevHitting.AVG += std::pow(player.calculations.zHitting.AVG - avgHitting.AVG, 2.0);
    });
    stddevHitting.AVG = std::sqrt(1. / double(numHitters) * stddevHitting.AVG);

    FBBPlayerDataService::ForEachValidHitter([&](FBBPlayer& player) {
        player.calculations.zHitting.AVG = (player.calculations.zHitting.AVG - avgHitting.AVG) / stddevHitting.AVG;
    });

    FBBPlayerDataService::ForEachValidHitter([&](FBBPlayer& player) {
        player.calculations.zScore  = 0;
        if (FBBLeaugeSettings::Instance().categories.hitting.HR) {
            player.calculations.zScore += player.calculations.zHitting.HR;
        }
        if (FBBLeaugeSettings::Instance().categories.hitting.R) {
            player.calculations.zScore += player.calculations.zHitting.R;
        }
        if (FBBLeaugeSettings::Instance().categories.hitting.RBI) {
            player.calculations.zScore += player.calculations.zHitting.RBI;
        }
        if (FBBLeaugeSettings::Instance().categories.hitting.SB) {
            player.calculations.zScore += player.calculations.zHitting.SB;
        }
        if (FBBLeaugeSettings::Instance().categories.hitting.AVG) {
            player.calculations.zScore  += player.calculations.zHitting.AVG;
        }
    });

    /////////////////////////

    double numPitchers = 0;

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
    FBBPlayerDataService::ForEachValidPitcher([&](const FBBPlayer& player) {
        numPitchers++;
        sumPitching.W += player.spProjection->pitching.W;
        sumPitching.L += player.spProjection->pitching.L;
        sumPitching.GS += player.spProjection->pitching.GS;
        sumPitching.G += player.spProjection->pitching.G;
        sumPitching.SV += player.spProjection->pitching.SV;
        sumPitching.IP += player.spProjection->pitching.IP;
        sumPitching.H += player.spProjection->pitching.H;
        sumPitching.ER += player.spProjection->pitching.ER;
        sumPitching.HR += player.spProjection->pitching.HR;
        sumPitching.SO += player.spProjection->pitching.SO;
        sumPitching.BB += player.spProjection->pitching.BB;
    });

    PerPitching avgPitching;
    avgPitching.W = sumPitching.W / numPitchers;
    avgPitching.L = sumPitching.L / numPitchers;
    avgPitching.GS = sumPitching.GS / numPitchers;
    avgPitching.G = sumPitching.G / numPitchers;
    avgPitching.SV = sumPitching.SV / numPitchers;
    avgPitching.IP = sumPitching.IP / numPitchers;
    avgPitching.H = sumPitching.H / numPitchers;
    avgPitching.ER = sumPitching.ER / numPitchers;
    avgPitching.HR = sumPitching.HR / numPitchers;
    avgPitching.SO = sumPitching.SO / numPitchers;
    avgPitching.BB = sumPitching.BB / numPitchers;
    avgPitching.ERA = sumPitching.ER / (9 * sumPitching.IP);
    avgPitching.WHIP = (sumPitching.BB + sumPitching.H) / sumPitching.IP;

    PerPitching stddevPitching;
    FBBPlayerDataService::ForEachValidPitcher([&](const FBBPlayer& player) {
        stddevPitching.W += std::pow(player.spProjection->pitching.W - avgPitching.W, 2.0);
        stddevPitching.L += std::pow(player.spProjection->pitching.L - avgPitching.L, 2.0);
        stddevPitching.GS += std::pow(player.spProjection->pitching.GS - avgPitching.GS, 2.0);
        stddevPitching.G += std::pow(player.spProjection->pitching.G - avgPitching.G, 2.0);
        stddevPitching.SV += std::pow(player.spProjection->pitching.SV - avgPitching.SV, 2.0);
        stddevPitching.IP += std::pow(player.spProjection->pitching.IP - avgPitching.IP, 2.0);
        stddevPitching.H += std::pow(player.spProjection->pitching.H - avgPitching.H, 2.0);
        stddevPitching.ER += std::pow(player.spProjection->pitching.ER - avgPitching.ER, 2.0);
        stddevPitching.HR += std::pow(player.spProjection->pitching.HR - avgPitching.HR, 2.0);
        stddevPitching.SO += std::pow(player.spProjection->pitching.SO - avgPitching.SO, 2.0);
        stddevPitching.BB += std::pow(player.spProjection->pitching.BB - avgPitching.BB, 2.0);
        stddevPitching.ERA += std::pow(player.spProjection->pitching.ERA() - avgPitching.ERA, 2.0);
        stddevPitching.WHIP += std::pow(player.spProjection->pitching.WHIP() - avgPitching.WHIP, 2.0);
    });

    stddevPitching.W = std::sqrt(1. / double(numPitchers) * stddevPitching.W);
    stddevPitching.L = std::sqrt(1. / double(numPitchers) * stddevPitching.L);
    stddevPitching.GS = std::sqrt(1. / double(numPitchers) * stddevPitching.GS);
    stddevPitching.G = std::sqrt(1. / double(numPitchers) * stddevPitching.G);
    stddevPitching.SV = std::sqrt(1. / double(numPitchers) * stddevPitching.SV);
    stddevPitching.IP = std::sqrt(1. / double(numPitchers) * stddevPitching.IP);
    stddevPitching.H = std::sqrt(1. / double(numPitchers) * stddevPitching.H);
    stddevPitching.ER = std::sqrt(1. / double(numPitchers) * stddevPitching.ER);
    stddevPitching.HR = std::sqrt(1. / double(numPitchers) * stddevPitching.HR);
    stddevPitching.SO = std::sqrt(1. / double(numPitchers) * stddevPitching.SO);
    stddevPitching.BB = std::sqrt(1. / double(numPitchers) * stddevPitching.BB);
    stddevPitching.ERA = std::sqrt(1. / double(numPitchers) * stddevPitching.ERA);
    stddevPitching.WHIP = std::sqrt(1. / double(numPitchers) * stddevPitching.WHIP);

    FBBPlayerDataService::ForEachValidPitcher([&](FBBPlayer& player) {
        player.calculations.zPitching.W = (player.spProjection->pitching.W - avgPitching.W) / stddevPitching.W;
        player.calculations.zPitching.SV = (player.spProjection->pitching.SV - avgPitching.SV) / stddevPitching.SV;
        player.calculations.zPitching.SO = (player.spProjection->pitching.SO - avgPitching.SO) / stddevPitching.SO;
        player.calculations.zPitching.ERA = (player.spProjection->pitching.ERA() - avgPitching.ERA) / stddevPitching.ERA;
        player.calculations.zPitching.WHIP = (player.spProjection->pitching.WHIP() - avgPitching.WHIP) / stddevPitching.WHIP;
    });

    sumPitching.ERA = 0;
    sumPitching.WHIP = 0;
    FBBPlayerDataService::ForEachValidPitcher([&](FBBPlayer& player) {
        player.calculations.zPitching.ERA *= player.spProjection->pitching.IP;
        player.calculations.zPitching.WHIP *= player.spProjection->pitching.IP;
        sumPitching.ERA += player.calculations.zPitching.ERA;
        sumPitching.WHIP += player.calculations.zPitching.WHIP;
    });

    stddevPitching.ERA = 0;
    stddevPitching.WHIP = 0;
    FBBPlayerDataService::ForEachValidPitcher([&](FBBPlayer& player) {
        stddevPitching.ERA += std::pow(player.calculations.zPitching.ERA - avgPitching.ERA, 2.0);
        stddevPitching.WHIP += std::pow(player.calculations.zPitching.WHIP - avgPitching.WHIP, 2.0);
    });
    stddevPitching.ERA = std::sqrt(1. / double(numPitchers) * stddevPitching.ERA);
    stddevPitching.WHIP = std::sqrt(1. / double(numPitchers) * stddevPitching.WHIP);

    FBBPlayerDataService::ForEachValidPitcher([&](FBBPlayer& player) {
        player.calculations.zPitching.ERA = -1. * (player.calculations.zPitching.ERA - avgPitching.ERA) / stddevPitching.ERA;
        player.calculations.zPitching.WHIP = -1. * (player.calculations.zPitching.WHIP - avgPitching.WHIP) / stddevPitching.WHIP;
    });

    FBBPlayerDataService::ForEachValidPitcher([&](FBBPlayer& player) {
        player.calculations.zScore = 0;
        if (FBBLeaugeSettings::Instance().categories.pitching.ERA) {
            player.calculations.zScore += player.calculations.zPitching.ERA;
        }
        if (FBBLeaugeSettings::Instance().categories.pitching.SO) {
            player.calculations.zScore += player.calculations.zPitching.SO;
        }
        if (FBBLeaugeSettings::Instance().categories.pitching.SV) {
            player.calculations.zScore += player.calculations.zPitching.SV;
        }
        if (FBBLeaugeSettings::Instance().categories.pitching.W) {
            player.calculations.zScore += player.calculations.zPitching.W;
        }
        if (FBBLeaugeSettings::Instance().categories.pitching.WHIP) {
            player.calculations.zScore += player.calculations.zPitching.WHIP;
        }
    });
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

    CalculateZScores();

    emit EndProjectionsUpdated();
}
