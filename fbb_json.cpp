#include <QCoreApplication>

#include <cstdint>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QMap>

#include <vector>
#include <iostream>
#include <algorithm>
#include <map>

struct Hitter
{
    QString _NAME;
    QString _TEAM;
    QString _ID;
    uint32_t _G;
    uint32_t _PA;
    uint32_t _AB;
    uint32_t _H;
    uint32_t _2B;
    uint32_t _3B;
    uint32_t _HR;
    uint32_t _R;
    uint32_t _RBI;
    uint32_t _xBB;
    uint32_t _SO;
    uint32_t _HBP;
    uint32_t _SB;
    uint32_t _CS;
};

static std::vector<Hitter> hitters_steamer()
{
    std::vector<Hitter> ret;

    // Open file
    QFile inputFile(":/data/2019-hitters-fan.csv");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream textStream(&inputFile);

    // Tokenize header data
    QStringList parsed = textStream.readLine().split(",");
    parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

    enum SteamerHitter
    {
        STEAMER_HITTER_NAME,
        STEAMER_HITTER_TEAM,
        STEAMER_HITTER_G,
        STEAMER_HITTER_PA,
        STEAMER_HITTER_AB,
        STEAMER_HITTER_H,
        STEAMER_HITTER_2B,
        STEAMER_HITTER_3B,
        STEAMER_HITTER_HR,
        STEAMER_HITTER_R,
        STEAMER_HITTER_RBI,
        STEAMER_HITTER_BB,
        STEAMER_HITTER_SO,
        STEAMER_HITTER_HBP,
        STEAMER_HITTER_SB,
        STEAMER_HITTER_CS,
        STEAMER_HITTER_BLANK_1,
        STEAMER_HITTER_AVG,
        STEAMER_HITTER_OBP,
        STEAMER_HITTER_SLG,
        STEAMER_HITTER_OPS,
        STEAMER_HITTER_WOBA,
        STEAMER_HITTER_BLANK_2,
        STEAMER_HITTER_WRC,
        STEAMER_HITTER_BSR,
        STEAMER_HITTER_FLD,
        STEAMER_HITTER_BLANK_3,
        STEAMER_HITTER_OFF,
        STEAMER_HITTER_DEF,
        STEAMER_HITTER_WAR,
        STEAMER_HITTER_BLANK_4,
        STEAMER_HITTER_ADP,
        STEAMER_HITTER_PLAYER_ID,
    };

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

    // Loop rows
    while (!textStream.atEnd()) {

        // Tokenize this row
        QStringList parsed = textStream.readLine().split(",");
        parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

        Hitter hitter;
        hitter._NAME = parsed[FAN_HITTER_NAME];
        hitter._TEAM = parsed[FAN_HITTER_TEAM];
        hitter._ID   = parsed[FAN_HITTER_PLAYER_ID];
        hitter._G    = parsed[FAN_HITTER_G].toInt();
        hitter._PA   = parsed[FAN_HITTER_PA].toInt();
        hitter._AB   = parsed[FAN_HITTER_AB].toInt();
        hitter._H    = parsed[FAN_HITTER_H].toInt();
        hitter._2B   = parsed[FAN_HITTER_2B].toInt();
        hitter._3B   = parsed[FAN_HITTER_3B].toInt();
        hitter._HR   = parsed[FAN_HITTER_HR].toInt();
        hitter._R    = parsed[FAN_HITTER_R].toInt();
        hitter._RBI  = parsed[FAN_HITTER_RBI].toInt();
        hitter._xBB  = parsed[FAN_HITTER_BB].toInt();
        hitter._SO   = parsed[FAN_HITTER_SO].toInt();
        hitter._HBP  = parsed[FAN_HITTER_HBP].toInt();
        hitter._SB   = parsed[FAN_HITTER_SB].toInt();
        hitter._CS   = parsed[FAN_HITTER_CS].toInt();
        ret.push_back(hitter);
    }

    return ret;
}

struct Pitcher
{
    QString _NAME;
    QString _TEAM;
    QString _ID;
    uint32_t _W;
    uint32_t _L;
    uint32_t _ERA;
    uint32_t _GS;
    uint32_t _G;
    uint32_t _SV;
    uint32_t _IP;
    uint32_t _H;
    uint32_t _ER;
    uint32_t _HR;
    uint32_t _SO;
    uint32_t _xBB;
};

static std::vector<Pitcher> pitchers_steamer()
{
    // Log
    // GlobalLogger::AppendMessage("Loading hitting projections...");

    std::vector<Pitcher> ret;

    // Open file
    QFile inputFile(":/data/2019-pitchers-fan.csv");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream textStream(&inputFile);

    // Tokenize header data
    QStringList parsed = textStream.readLine().split(",");
    parsed.replaceInStrings("\"", "", Qt::CaseInsensitive);

    enum SteamerPitcher
    {
        STEAMER_PITCHER_NAME,
        STEAMER_PITCHER_TEAM,
        STEAMER_PITCHER_W,
        STEAMER_PITCHER_L,
        STEAMER_PITCHER_ERA,
        STEAMER_PITCHER_GS,
        STEAMER_PITCHER_G,
        STEAMER_PITCHER_SV,
        STEAMER_PITCHER_IP,
        STEAMER_PITCHER_H,
        STEAMER_PITCHER_ER,
        STEAMER_PITCHER_HR,
        STEAMER_PITCHER_SO,
        STEAMER_PITCHER_BB,
        STEAMER_PITCHER_WHIP,
        STEAMER_PITCHER_Kp9,
        STEAMER_PITCHER_BBp9,
        STEAMER_PITCHER_FIP,
        STEAMER_PITCHER_WAR,
        STEAMER_PITCHER_RA9mWAR,
        STEAMER_PITCHER_ADP,
        STEAMER_PITCHER_PLAYER_ID,
    };

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

        Pitcher pitcher;
        pitcher._NAME = parsed[FAN_PITCHER_NAME];
        pitcher._TEAM = parsed[FAN_PITCHER_TEAM];
        pitcher._ID   = parsed[FAN_PITCHER_PLAYER_ID];
        pitcher._W    = parsed[FAN_PITCHER_W].toInt();
        pitcher._L    = parsed[FAN_PITCHER_L].toInt();
        pitcher._ERA  = parsed[FAN_PITCHER_ERA].toInt();
        pitcher._GS   = parsed[FAN_PITCHER_GS].toInt();
        pitcher._G    = parsed[FAN_PITCHER_G].toInt();
        pitcher._SV   = parsed[FAN_PITCHER_SV].toInt();
        pitcher._IP   = parsed[FAN_PITCHER_IP].toInt();
        pitcher._H    = parsed[FAN_PITCHER_H].toInt();
        pitcher._ER   = parsed[FAN_PITCHER_ER].toInt();
        pitcher._HR   = parsed[FAN_PITCHER_HR].toInt();
        pitcher._SO   = parsed[FAN_PITCHER_SO].toInt();
        pitcher._xBB  = parsed[FAN_PITCHER_BB].toInt();
        ret.push_back(pitcher);
    }

    return ret;
}

struct Apperance
{
    QString _POS;
    uint32_t _INN;
};

std::map<QString, std::vector<Apperance>> appearances()
{
    std::map<QString, std::vector<Apperance>> ret;

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

        Apperance app;
        app._POS  = parsed[APPEARANCE_POS];
        app._INN  = parsed[APPEARANCE_INN].toInt();

        ret[id].push_back(app);
    }

    return ret;
}

int main(int argc, char *argv[])
{
    std::map<QString, std::vector<Apperance>> mapApp = appearances();

    std::vector<Hitter> vecHitters = hitters_steamer();
    std::vector<Pitcher> vecPitchers = pitchers_steamer();

    std::vector<const Hitter*> noMatch;

    std::cout << "name, id, pa" << std::endl;

    for (const Hitter& hitter : vecHitters)
    {
        auto itr = std::find_if(mapApp.begin(), mapApp.end(), [&](const auto& pair){
            return pair.first == hitter._ID;
        });

        if (itr == mapApp.end()) {

            std::cout << qPrintable(hitter._NAME) << ",";
            std::cout << qPrintable(hitter._ID) << ",";
            std::cout << hitter._PA << std::endl;
        }
    }
}

