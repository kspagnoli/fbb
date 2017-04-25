#include "FBB/FBBProjectionService.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"
#include "FBB/FBBPlayerDataService.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

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
        QSharedPointer<FBBPlayer> spPlayer = FBBPlayerDataService::GetPlayer(parsed[LUT["playerid"]]);
        spPlayer->name = parsed[LUT["Name"]];
        spPlayer->team = ToFBBTeam(parsed[LUT["Team"]]);

        // Create projections
        QSharedPointer<FBBPlayer::Projection> spProjection = QSharedPointer<FBBPlayer::Projection>::create();

        // Load stats
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
        spPlayer->spProjection = std::move(spProjection);
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
        QSharedPointer<FBBPlayer> spPlayer = FBBPlayerDataService::GetPlayer(parsed[LUT["playerid"]]);
        spPlayer->name = parsed[LUT["Name"]];
        spPlayer->team = ToFBBTeam(parsed[LUT["Team"]]);
        
        // Create projections
        QSharedPointer<FBBPlayer::Projection> spProjection = QSharedPointer<FBBPlayer::Projection>::create();

        // Load stats
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
        spProjection->pitching.ADP = parsed[LUT["ADP"]].toUInt();

        // Save to player
        spPlayer->spProjection = std::move(spProjection);
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

    emit EndProjectionsUpdated();
}
