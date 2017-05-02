#include "FBB/FBBPositionService.h"
#include "FBB/FBBPlayerDataService.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QMap>

FBBPositionService::FBBPositionService(QObject* parent)
{
    // Listen for settings changes
    connect(&FBBLeaugeSettings::Instance(), &FBBLeaugeSettings::SettingsChanged, this, [=] {
        LoadPositionData();
    });
}

void FBBPositionService::LoadPositionData()
{
    // Load min number
    const uint32_t positionEligibility = FBBLeaugeSettings::Instance().leauge.positionEligibility;

    // Open file
    QFile inputFile(":/data/2016-appearances.csv");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream textStream(&inputFile);

    // Tokenize header data
    QStringList parsed = textStream.readLine().split(",");
    parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

    // Stats to find
    QMap<QString, int32_t> LUT;
    LUT["Rk"] = parsed.indexOf("Rk");
    LUT["Name"] = parsed.indexOf("Name");
    LUT["Age"] = parsed.indexOf("Age");
    LUT["Tm"] = parsed.indexOf("Tm");
    LUT["Yrs"] = parsed.indexOf("Yrs");
    LUT["G"] = parsed.indexOf("G");
    LUT["GS"] = parsed.indexOf("GS");
    LUT["Batting"] = parsed.indexOf("Batting");
    LUT["Defense"] = parsed.indexOf("Defense");
    LUT["P"] = parsed.indexOf("P");
    LUT["C"] = parsed.indexOf("C");
    LUT["1B"] = parsed.indexOf("1B");
    LUT["2B"] = parsed.indexOf("2B");
    LUT["3B"] = parsed.indexOf("3B");
    LUT["SS"] = parsed.indexOf("SS");
    LUT["LF"] = parsed.indexOf("LF");
    LUT["CF"] = parsed.indexOf("CF");
    LUT["RF"] = parsed.indexOf("RF");
    LUT["OF"] = parsed.indexOf("OF");
    LUT["DH"] = parsed.indexOf("DH");
    LUT["PH"] = parsed.indexOf("PH");
    LUT["PR"] = parsed.indexOf("PR");

    // Loop rows
    while (!textStream.atEnd()) {

        // Tokenize this row
        QStringList parsed = textStream.readLine().split(",");
        parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

        // Lazily get player
        FBBPlayer* spPlayer = FBBPlayerDataService::GetPlayerFromBaseballReference(parsed[LUT["Name"]], parsed[LUT["Tm"]]);
        if (!spPlayer) {
            continue;
        }

        // Hitting positions
        if (parsed[LUT["C"]].toUInt() >= positionEligibility) { spPlayer->eligablePositions |= FBB_POSITION_C; }
        if (parsed[LUT["1B"]].toUInt() >= positionEligibility) { spPlayer->eligablePositions |= FBB_POSITION_1B; }
        if (parsed[LUT["2B"]].toUInt() >= positionEligibility) { spPlayer->eligablePositions |= FBB_POSITION_2B; }
        if (parsed[LUT["3B"]].toUInt() >= positionEligibility) { spPlayer->eligablePositions |= FBB_POSITION_3B; }
        if (parsed[LUT["SS"]].toUInt() >= positionEligibility) { spPlayer->eligablePositions |= FBB_POSITION_SS; }
        if (parsed[LUT["LF"]].toUInt() >= positionEligibility) { spPlayer->eligablePositions |= FBB_POSITION_LF; }
        if (parsed[LUT["CF"]].toUInt() >= positionEligibility) { spPlayer->eligablePositions |= FBB_POSITION_CF; }
        if (parsed[LUT["RF"]].toUInt() >= positionEligibility) { spPlayer->eligablePositions |= FBB_POSITION_RF; }
        if (parsed[LUT["DH"]].toUInt() >= positionEligibility) { spPlayer->eligablePositions |= FBB_POSITION_DH; }

        // Pitching positions
        if (parsed[LUT["P"]].toUInt() >= positionEligibility) { 
            spPlayer->eligablePositions |= FBB_POSITION_P;
        }
    }
}

FBBPositionService& FBBPositionService::Instance()
{
    static FBBPositionService* s_service = new FBBPositionService(qApp);
    return *s_service;
}
