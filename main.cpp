#include <QApplication>
#include <QLabel>
#include <QAbstractTableModel>
#include <QTableView>
#include <QMainWindow>
#include <QDockWidget>
#include <QStyleFactory>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QSplitter>
#include <QTableWidget>

#include <memory>
#include <cstdint>
#include <fstream> 
#include <string>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <tuple>
#include <bitset>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

namespace Leauge
{
    enum Enum
    {
        Unknown,
        AL,
        NL,
    };
}

namespace Division
{
    enum Enum
    {
        Unknown,
        ALEast,
        ALCentral,
        ALWest,
        NLEast,
        NLCentral,
        NLWest,
    };
}

namespace HittingPositions
{
    enum Enum
    {
        Catcher,          
        First,            
        Second,           
        Shortstop,        
        Third,            
        Outfield,         
        DesignatedHitter,

        Count
    };
}

namespace PitchingPositions
{
    enum Enum
    {
        Starter,
        Relief,

        Count
    };
}

struct Group
{
    Leauge::Enum Leauge;
    Division::Enum Division;
};

template <typename FnGet, typename Player>
float average(const FnGet& fnGet, const std::vector<Player>& players)
{
    double sum = std::accumulate(std::begin(players),std::end(players),double(0),[&](double sum, const Player& x)
    {
        return sum + double(fnGet(x));
    });

    return sum / float(players.size());
}

template <typename FnGet, typename Player>
float stdev(const FnGet& fnGet, const float& average, const std::vector<Player>& players)
{
    double accum = 0;
    std::for_each(std::begin(players),std::end(players),[&](const Player& x)
    {
        accum += (fnGet(x) - average) * (fnGet(x) - average);
    });

    return std::sqrt(accum / double(players.size()-1));
}

float score(float sample, float avg,float stdev)
{
    return (sample - avg) / stdev;
};

struct Hitter
{
   
    std::string name;
    std::string team;

    std::bitset<HittingPositions::Count> positions;

    uint32_t plateApperances = 0;
    uint32_t atBats = 0;
    uint32_t hits = 0;
    uint32_t homeRuns = 0;
    uint32_t runs = 0;
    uint32_t rBIs = 0;
    uint32_t stolenBases = 0;

    float average = 0;

    float zAverage = 0;
    float zHomeRuns = 0;
    float zRuns = 0;
    float zRBIs = 0;
    float zStolenBases = 0;

    // estimate
    float zScore = 0;
    float cost = 0;

    // comment
    std::string comment;
};

struct Pitcher
{
    std::string name;
    std::string team;

    std::bitset<PitchingPositions::Count> positions;

    float IP = 0;
    uint32_t SO = 0;
    uint32_t W = 0;
    uint32_t SV = 0;
    float ERA = 0;
    float WHIP = 0;

    float zSO = 0;
    float zW = 0;
    float zSV = 0;
    float zERA = 0;
    float zWHIP = 0;

    // estimate
    float zScore = 0;
    float cost = 0;

    // comment
    std::string comment;
};


struct Settings 
{
    static const size_t Owners = 12;

    static const size_t RosterMoney = 270;
    
    static const size_t RosterHitters = 14;
    static const size_t RosterPitchers = 10;

    static const size_t TotalHittersRostered = Owners*RosterHitters;
    static const size_t TotalPitchersRostered = Owners*RosterPitchers;
        
    static float HitterPitcherSplit() { return 0.667f; }

    static float HitterMoney() { return float(RosterMoney) * HitterPitcherSplit(); }
    static float PitcherMoney() { return RosterMoney * (1.f - HitterPitcherSplit()); }
};

static Group LookupTeamGroup(const std::string& teamName)
{

    static std::unordered_map<std::string, Group> s_LUT = {

        {"Orioles",{Leauge::AL,Division::ALEast}},
        {"Red Sox",{Leauge::AL,Division::ALEast}},
        {"Yankees",{Leauge::AL,Division::ALEast}},
        {"Rays",{Leauge::AL,Division::ALEast}},
        {"Blue Jays",{Leauge::AL,Division::ALEast}},
        {"Braves",{Leauge::NL,Division::NLEast}},
        {"Marlins",{Leauge::NL,Division::NLEast}},
        {"Mets",{Leauge::NL,Division::NLEast}},
        {"Phillies",{Leauge::NL,Division::NLEast}},
        {"Nationals",{Leauge::NL,Division::NLEast}},
        {"White Sox",{Leauge::AL,Division::ALCentral}},
        {"Indians",{Leauge::AL,Division::ALCentral}},
        {"Tigers",{Leauge::AL,Division::ALCentral}},
        {"Royals",{Leauge::AL,Division::ALCentral}},
        {"Twins",{Leauge::AL,Division::ALCentral}},
        {"Cubs",{Leauge::NL,Division::NLCentral}},
        {"Reds",{Leauge::NL,Division::NLCentral}},
        {"Brewers",{Leauge::NL,Division::NLCentral}},
        {"Pirates",{Leauge::NL,Division::NLCentral}},
        {"Cardinals",{Leauge::NL,Division::NLCentral}},
        {"Astros",{Leauge::AL,Division::ALWest}},
        {"Angels",{Leauge::AL,Division::ALWest}},
        {"Athletics",{Leauge::AL,Division::ALWest}},
        {"Mariners",{Leauge::AL,Division::ALWest}},
        {"Rangers",{Leauge::AL,Division::ALWest}},
        {"Diamondbacks",{Leauge::NL,Division::NLWest}},
        {"Rockies",{Leauge::NL,Division::NLWest}},
        {"Dodgers",{Leauge::NL,Division::NLWest}},
        {"Padres",{Leauge::NL,Division::NLWest}},
        {"Giants",{Leauge::NL,Division::NLWest}},

    };

    return s_LUT[teamName];
}

//
class HitterData 
{

public:

    HitterData() = default;

    void UpdateZScores()
    {
        // Helpers
        const auto fnGetAtBats = [](const Hitter& hitter) { return hitter.atBats; };
        const auto fnGetAverage = [](const Hitter& hitter) { return hitter.average; };
        const auto fnGetHomeRuns = [](const Hitter& hitter) { return hitter.homeRuns; };
        const auto fnGetRuns = [](const Hitter& hitter) { return hitter.runs; };
        const auto fnGetRBIs = [](const Hitter& hitter) { return hitter.rBIs; };
        const auto fnGetStolenBases = [](const Hitter& hitter) { return hitter.stolenBases; };

        // Averages
        const float avgAtBats = average(fnGetAtBats, vecHitters);
        const float avgAverage = average(fnGetAverage, vecHitters);
        const float avgHomeRuns = average(fnGetHomeRuns, vecHitters);
        const float avgRuns = average(fnGetRuns, vecHitters);
        const float avgRBIs = average(fnGetRBIs, vecHitters);
        const float avgStolenBases = average(fnGetStolenBases, vecHitters);

        // Standard devs
        const float stdevAtBats = stdev(fnGetAtBats, avgAtBats, vecHitters);
        const float stdevAverage = stdev(fnGetAverage, avgAverage, vecHitters);
        const float stdevHomeRuns = stdev(fnGetHomeRuns, avgHomeRuns, vecHitters);
        const float stdevRuns = stdev(fnGetRuns, avgRuns, vecHitters);
        const float stdevRBIs = stdev(fnGetRBIs, avgRBIs, vecHitters);
        const float stdevStolenBases = stdev(fnGetStolenBases, avgStolenBases, vecHitters);

        // zscore
        std::for_each(std::begin(vecHitters),std::end(vecHitters),[&](Hitter& hitter) {

            // per-state zscores
            hitter.zAverage = (hitter.atBats * score(hitter.average, avgAverage, stdevAverage) - stdevAtBats) / avgAtBats;
            hitter.zHomeRuns = score(hitter.homeRuns, avgHomeRuns, stdevHomeRuns);
            hitter.zRuns = score(hitter.runs, avgRuns, stdevRuns);
            hitter.zRBIs = score(hitter.rBIs, avgRBIs, stdevRBIs);
            hitter.zStolenBases = score(hitter.stolenBases, avgStolenBases, stdevStolenBases);

            // zscore summation
            hitter.zScore = hitter.zAverage + hitter.zHomeRuns + hitter.zRuns + hitter.zRBIs + hitter.zStolenBases;
        });

        // Sort by zScore
        std::sort(vecHitters.begin(), vecHitters.end(), [](const Hitter& lhs, const Hitter& rhs) {
            return lhs.zScore > rhs.zScore;
        });

        // Get the "replacement player"
        float zRplacement = vecHitters[Settings::TotalHittersRostered].zScore;

        // Scale all players based off the replacement player
        std::for_each(std::begin(vecHitters), std::end(vecHitters), [&](Hitter& hitter) {
            hitter.zScore -= zRplacement;
        });

        // Sum all positive zScores
        float sumPositiveZScores = 0;
        std::for_each(std::begin(vecHitters), std::end(vecHitters), [&](Hitter& hitter) {
            if (hitter.zScore > 0) {
                sumPositiveZScores += hitter.zScore;
            }
        });

        // Apply cost ratio
        static const float costRatio = (Settings::HitterMoney() / 14.f) * (Settings::TotalHittersRostered / sumPositiveZScores);
        std::for_each(std::begin(vecHitters), std::end(vecHitters), [&](Hitter& hitter) {
            hitter.cost = hitter.zScore * costRatio;
        });
    }

    // xxx
    std::vector<Hitter> vecHitters;

private:

};

class PitcherData
{

public:

    PitcherData() = default;

    void UpdateZScores()
    {      
        // Helpers
        const auto fnGetIP   = [](const Pitcher& pitcher) { return pitcher.IP;   };
        const auto fnGetSO   = [](const Pitcher& pitcher) { return pitcher.SO;   };
        const auto fnGetW    = [](const Pitcher& pitcher) { return pitcher.W;    };
        const auto fnGetSV   = [](const Pitcher& pitcher) { return pitcher.SV;   };
        const auto fnGetERA  = [](const Pitcher& pitcher) { return pitcher.ERA;  };
        const auto fnGetWHIP = [](const Pitcher& pitcher) { return pitcher.WHIP; };

        // Averages
        const float avgIP   = average(fnGetIP,   vecPitchers);
        const float avgSO   = average(fnGetSO,   vecPitchers);
        const float avgW    = average(fnGetW,    vecPitchers);
        const float avgSV   = average(fnGetSV,   vecPitchers);
        const float avgERA  = average(fnGetERA,  vecPitchers);
        const float avgWHIP = average(fnGetWHIP, vecPitchers);

        // Standard devs
        const float stdevIP   = stdev(fnGetIP,   avgIP,   vecPitchers);
        const float stdevSO   = stdev(fnGetSO,   avgSO,   vecPitchers);
        const float stdevW    = stdev(fnGetW,    avgW,    vecPitchers);
        const float stdevSV   = stdev(fnGetSV,   avgSV,   vecPitchers);
        const float stdevERA  = stdev(fnGetERA,  avgERA,  vecPitchers);
        const float stdevWHIP = stdev(fnGetWHIP, avgWHIP, vecPitchers);
        
        // zscore
        std::for_each(std::begin(vecPitchers), std::end(vecPitchers), [&](Pitcher& pitcher) {

            // per-state zscores
            pitcher.zSO = score(pitcher.SO, avgSO, stdevSO);
            pitcher.zW = score(pitcher.W, avgW, stdevW);
            pitcher.zSV = score(pitcher.SV, avgSV, stdevSV);
            pitcher.zERA = -(pitcher.IP * score(pitcher.ERA, avgERA, stdevERA) - stdevIP) / avgIP;
            pitcher.zWHIP = -(pitcher.IP * score(pitcher.WHIP, avgWHIP, stdevWHIP) - stdevIP) / avgIP;

            // zscore summation
            pitcher.zScore = pitcher.zSO + pitcher.zW + pitcher.zSV + pitcher.zERA + pitcher.zWHIP;
        });

        // Sort by zScore
        std::sort(vecPitchers.begin(), vecPitchers.end(), [](const Pitcher& lhs,const Pitcher& rhs) {
            return lhs.zScore > rhs.zScore;
        });

        // Get the "replacement player"
        float zRplacement = vecPitchers[Settings::TotalPitchersRostered].zScore;

        // Scale all players based off the replacement player
        std::for_each(std::begin(vecPitchers), std::end(vecPitchers), [&](Pitcher& pitcher) {
            pitcher.zScore -= zRplacement;
        });

        // Sum all positive zScores
        float sumPositiveZScores = 0;
        std::for_each(std::begin(vecPitchers), std::end(vecPitchers), [&](Pitcher& pitcher) {
            if (pitcher.zScore > 0) {
                sumPositiveZScores += pitcher.zScore;
            }
        });

        // Apply cost ratio
        static const float costRatio = (Settings::PitcherMoney() / 10.f) * (Settings::TotalPitchersRostered / sumPositiveZScores);
        std::for_each(std::begin(vecPitchers), std::end(vecPitchers), [&](Pitcher& pitcher) {
            pitcher.cost = pitcher.zScore * costRatio;
        });
    }

    // xxx
    std::vector<Pitcher> vecPitchers;

private:

};

//
std::unordered_map<std::string, uint32_t> streamer_hitters = 
{
    {"Name",0},
    {"Team",0},
    {"PA",0},
    {"AB",0},
    {"H",0},
    {"HR",0},
    {"R",0},
    {"RBI",0},
    {"SB",0},
};

std::unordered_map<std::string,uint32_t> streamer_pitchers =
{
    {"Name",0},
    {"Team",0},
    {"IP",0},
    {"SO",0},
    {"ERA",0},
    {"WHIP",0},
    {"W",0},
    {"SV",0},
};

class HitterTableModel : public QAbstractTableModel
{

public:

    // Columns
    enum COLUMN
    {
        COLUMN_RANK,
        COLUMN_NAME,
        COLUMN_TEAM,
        COLUMN_POSITION,
        COLUMN_AB,
        COLUMN_AVG,
        COLUMN_HR,
        COLUMN_R,
        COLUMN_RBI,
        COLUMN_SB,
        COLUMN_Z,
        COLUMN_ESTIMATE,
        COLUMN_COMMENT,

        COLUMN_COUNT
    };

    //
    HitterTableModel(const std::shared_ptr<HitterData>& spHitterData)
        : m_spHitterData(spHitterData)
    {
    }

    //
    int rowCount(const QModelIndex &) const override
    {
        return m_spHitterData->vecHitters.size();
    }

    //
    int columnCount(const QModelIndex &) const override
    {
        return COLUMN_COUNT;
    }
    
    //
    QVariant data(const QModelIndex& index, int role) const override
    {
        if (!index.isValid()) {
            return QVariant();
        }

        const size_t i = index.row();
        const Hitter& hitter = m_spHitterData->vecHitters.at(index.row());

        if (role == Qt::DisplayRole) {

            switch (index.column()) 
            {
            case COLUMN_RANK:
                return index.row() + 1;
            case COLUMN_NAME:
                return QString::fromStdString(hitter.name);
            case COLUMN_TEAM:
                return QString::fromStdString(hitter.team);
            case COLUMN_AB:
                return hitter.atBats;
            case COLUMN_AVG:
                return QString::number(hitter.average, 'f', 3);
            case COLUMN_HR:
                return hitter.homeRuns;
            case COLUMN_R:
                return hitter.runs;
            case COLUMN_RBI:
                return hitter.rBIs;
            case COLUMN_SB:
                return hitter.stolenBases;
            case COLUMN_Z:
                return QString::number(hitter.zScore,'f',3);
            case COLUMN_ESTIMATE:
                return QString("$%1").arg(QString::number(hitter.cost, 'f', 2));
            case COLUMN_COMMENT:
                return QString::fromStdString(hitter.comment);
            }
        }

        else if(role == Qt::TextAlignmentRole) {

            switch(index.column())
            {
            case COLUMN_NAME:
            case COLUMN_TEAM:
            case COLUMN_COMMENT:
                return Qt::AlignmentFlag(int(Qt::AlignLeft) | int(Qt::AlignVCenter));
            default:
                return Qt::AlignmentFlag(int(Qt::AlignRight) | int(Qt::AlignVCenter));
            }
        }

        else if(role == Qt::ToolTipRole) {

            switch(index.column())
            {
            case COLUMN_Z:
                return QString("zAVG: %1\nzR:   %2\nzRBI: %3\nzHR:  %4\nzSB:  %5")
                    .arg(hitter.zAverage)
                    .arg(hitter.zRuns)
                    .arg(hitter.zRBIs)
                    .arg(hitter.zHomeRuns)
                    .arg(hitter.zStolenBases);
            }
        }

        return QVariant();
    }

    //
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (orientation == Qt::Horizontal) {

            if (role == Qt::DisplayRole) {

                switch (section)
                {
                case COLUMN_RANK:
                    return "#";
                case COLUMN_NAME:
                    return "Name";
                case COLUMN_TEAM:
                    return "Team";
                case COLUMN_AB:
                    return "AB";
                case COLUMN_AVG:
                    return "AVG";
                case COLUMN_HR:
                    return "HR";
                case COLUMN_R:
                    return "R";
                case COLUMN_RBI:
                    return "RBI";
                case COLUMN_SB:
                    return "SB";
                case COLUMN_ESTIMATE:
                    return "$";
                case COLUMN_Z:
                    return "zScore";
                case COLUMN_COMMENT:
                    return "Comment";
                }
            }
        } 

        return QVariant();
    }

    // 
    const std::shared_ptr<HitterData> getHitterData() const
    {
        return m_spHitterData;
    }

private:

    std::shared_ptr<HitterData> m_spHitterData;
};


class PitcherTableModel: public QAbstractTableModel
{

public:

    // Columns
    enum COUNT
    {
        COLUMN_RANK,
        COLUMN_NAME,
        COLUMN_TEAM,
        COLUMN_IP,
        COLUMN_SO,
        COLUMN_ERA,
        COLUMN_WHIP,
        COLUMN_W,
        COLUMN_SV,
        COLUMN_Z,
        COLUMN_ESTIMATE,
        COLUMN_COMMENT,

        COLUMN_COUNT
    };

    //
    PitcherTableModel(const std::shared_ptr<PitcherData>& spPitcherData)
        : m_spPitcherData(spPitcherData)
    {
    }

    //
    int rowCount(const QModelIndex &) const override
    {
        return m_spPitcherData->vecPitchers.size();
    }

    //
    int columnCount(const QModelIndex &) const override
    {
        return COLUMN_COUNT;
    }

    //
    QVariant data(const QModelIndex& index,int role) const override
    {
        if(!index.isValid()) {
            return QVariant();
        }

        const size_t i = index.row();
        const Pitcher& pitcher = m_spPitcherData->vecPitchers.at(index.row());

        if(role == Qt::DisplayRole) {

            switch(index.column())
            {
            case COLUMN_RANK:
                return index.row() + 1;
            case COLUMN_NAME:
                return QString::fromStdString(pitcher.name);
            case COLUMN_TEAM:
                return QString::fromStdString(pitcher.team);
            case COLUMN_IP:
                return QString::number(pitcher.IP, 'f', 1);
            case COLUMN_SO:
                return pitcher.SO;
            case COLUMN_ERA:
                return QString::number(pitcher.ERA, 'f', 3);
            case COLUMN_WHIP:
                return QString::number(pitcher.WHIP, 'f', 3);
            case COLUMN_W:
                return pitcher.W;
            case COLUMN_SV:
                return pitcher.SV;
            case COLUMN_Z:
                return QString::number(pitcher.zScore,'f',3);
            case COLUMN_ESTIMATE:
                return QString("$%1").arg(QString::number(pitcher.cost,'f',2));
            case COLUMN_COMMENT:
                return QString::fromStdString(pitcher.comment);
            }
        }

        else if(role == Qt::TextAlignmentRole) {

            switch(index.column())
            {
            case COLUMN_NAME:
            case COLUMN_TEAM:
            case COLUMN_COMMENT:
                return Qt::AlignmentFlag(int(Qt::AlignLeft) | int(Qt::AlignVCenter));
            default:
                return Qt::AlignmentFlag(int(Qt::AlignRight) | int(Qt::AlignVCenter));
            }
        }

        else if(role == Qt::ToolTipRole) {

            // switch(index.column())
            // {
            // case Z_SCORE:
            //     return QString("zAVG: %1\nzR:   %2\nzRBI: %3\nzHR:  %4\nzSB:  %5")
            //         .arg(pitcher.zAverage)
            //         .arg(pitcher.zRuns)
            //         .arg(pitcher.zRBIs)
            //         .arg(pitcher.zHomeRuns)
            //         .arg(pitcher.zStolenBases);
            // }
        }

        return QVariant();
    }

    //
    QVariant headerData(int section,Qt::Orientation orientation,int role) const override
    {
        if(orientation == Qt::Horizontal) {

            if(role == Qt::DisplayRole) {

                switch(section)
                {
                case COLUMN_RANK:
                    return "#";
                case COLUMN_NAME:
                    return "Name";
                case COLUMN_TEAM:
                    return "Team";
                case COLUMN_IP:
                    return "IP";
                case COLUMN_SO:
                    return "KO";
                case COLUMN_ERA:
                    return "ERA";
                case COLUMN_WHIP:
                    return "WHIP";
                case COLUMN_W:
                    return "W";
                case COLUMN_SV:
                    return "SV";
                case COLUMN_Z:
                    return "zScore";
                case COLUMN_ESTIMATE:
                    return "$";
                case COLUMN_COMMENT:
                    return "Comment";
                }
            }
        }

        return QVariant();
    }

    // 
    const std::shared_ptr<PitcherData> getPitcherData() const
    {
        return m_spPitcherData;
    }

private:

    std::shared_ptr<PitcherData> m_spPitcherData;
};


// 
class HitterSortFilterProxyModel: public QSortFilterProxyModel
{
public:

    HitterSortFilterProxyModel(HitterTableModel* hittleTableModel)
        : m_hittleTableModel(hittleTableModel)
    {
        QSortFilterProxyModel::setSourceModel(m_hittleTableModel);
    }

    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override
    {
        auto column = left.column();
        if (column != right.column()) {
            return false;
        }

        // lookup hitters
        const Hitter& leftHitter  = m_hittleTableModel->getHitterData()->vecHitters.at(left.row());
        const Hitter& rightHitter = m_hittleTableModel->getHitterData()->vecHitters.at(right.row());

        QVariant leftData = sourceModel()->data(left);
        QVariant rightData = sourceModel()->data(right);

        switch (column)
        {
        case HitterTableModel::COLUMN_AVG:
            return leftHitter.average < rightHitter.average;
        case HitterTableModel::COLUMN_Z:
            return leftHitter.zScore < rightHitter.zScore;
        case HitterTableModel::COLUMN_ESTIMATE:
            return leftHitter.cost < rightHitter.cost;
        }

        return leftData < rightData;
    }

    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override
    {
        const Hitter& hitter = m_hittleTableModel->getHitterData()->vecHitters.at(sourceRow);

        // Hax
        // if (m_onlyShowAvailablePlayers && hitter.Get<Hitter> == false) {
        //     return false;
        // }

        return true;
    }

public slots:

    void OnlyShowAvailablePlayers(bool checked)
    {
        m_onlyShowAvailablePlayers = checked;

        filterChanged();
    }

private:

    HitterTableModel* m_hittleTableModel;

    // filters
    bool m_onlyShowAvailablePlayers;
};


// 
class PitcherSortFilterProxyModel: public QSortFilterProxyModel
{
public:

    PitcherSortFilterProxyModel(PitcherTableModel* pitcherTableModel)
        : m_pitcherTableModel(pitcherTableModel)
    {
        QSortFilterProxyModel::setSourceModel(m_pitcherTableModel);
    }

    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override
    {
        auto column = left.column();
        if(column != right.column()) {
            return false;
        }

        const Pitcher& leftPitcher  = m_pitcherTableModel->getPitcherData()->vecPitchers.at(left.row());
        const Pitcher& rightPitcher = m_pitcherTableModel->getPitcherData()->vecPitchers.at(right.row());

        QVariant leftData = sourceModel()->data(left);
        QVariant rightData = sourceModel()->data(right);

        switch(column)
        {
        case PitcherTableModel::COLUMN_WHIP:
            return leftPitcher.WHIP < rightPitcher.WHIP;
        case PitcherTableModel::COLUMN_ERA:
            return leftPitcher.ERA < rightPitcher.ERA;
        case PitcherTableModel::COLUMN_Z:
            return leftPitcher.zScore < rightPitcher.zScore;
        case PitcherTableModel::COLUMN_ESTIMATE:
            return leftPitcher.cost < rightPitcher.cost;
        }

        return leftData < rightData;
    }

    bool filterAcceptsRow(int sourceRow,const QModelIndex& sourceParent) const override
    {
        return true;
    }

public slots:

    void OnlyShowAvailablePlayers(bool checked)
    {
        m_onlyShowAvailablePlayers = checked;

        filterChanged();
    }

private:

    PitcherTableModel* m_pitcherTableModel;

    // filters
    bool m_onlyShowAvailablePlayers;
};


//
class MainWindow : public QMainWindow
{

public:

    MainWindow(const std::shared_ptr<HitterData>& hitterData, const std::shared_ptr<PitcherData>& pitcherData)
        : m_hitterData(hitterData)
        , m_pitcherData(pitcherData)
    {
        QWidget* central = new QWidget();

        // Hitter/Pitcher Layout
        QVBoxLayout* vBoxLayout = new QVBoxLayout();


        // Hitter/Pitcher Tab View
        QTabWidget* tabs = new QTabWidget(this);
        tabs->addTab(GetHitterTableView(), "Hitters");
        tabs->addTab(GetPitcherTableView(), "Pitchers");
        vBoxLayout->addWidget(tabs);

        // Hitter/Pitcher toolbar
        QToolBar* toolbar = new QToolBar("Toolbar");
        QAction* action = new QAction(this);
        action->setText(tr("Filter Available"));
        action->setCheckable(true);
        toolbar->addAction(action);
        toolbar->setFloatable(false);
        toolbar->setMovable(false);
        QMainWindow::addToolBar(toolbar);

        // Set as main window
        QMainWindow::setCentralWidget(central);
        central->setLayout(vBoxLayout);

        // Create menu bar
        QMenuBar* menuBar = new QMenuBar();
        menuBar->addAction("MENU BAR");
        QMainWindow::setMenuBar(menuBar);

        // Show me
        QMainWindow::show();
    }

    QTableView* GetHitterTableView()
    {
        // table model
        HitterTableModel* hittleTableModel = new HitterTableModel(m_hitterData);

        // sort/filter proxy model
        HitterSortFilterProxyModel* hitterSortFilterProxyModel = new HitterSortFilterProxyModel(hittleTableModel);

        // table view
        QTableView* tableView = new QTableView();
        tableView->setModel(hitterSortFilterProxyModel);
        tableView->setSortingEnabled(true);
        hitterSortFilterProxyModel->sort(HitterTableModel::COLUMN_RANK, Qt::AscendingOrder);
        
        return FormatTable(tableView);
    }

    // 
    QTableView* GetPitcherTableView()
    {
        // table model
        PitcherTableModel* pitcherTableModel = new PitcherTableModel(m_pitcherData);

        // sort/filter proxy model
        PitcherSortFilterProxyModel* pitcherSortFilterProxyModel = new PitcherSortFilterProxyModel(pitcherTableModel);

        // table view
        QTableView* tableView = new QTableView();
        tableView->setModel(pitcherSortFilterProxyModel);
        tableView->setSortingEnabled(true);
        pitcherSortFilterProxyModel->sort(PitcherTableModel::COLUMN_RANK, Qt::AscendingOrder);

        return FormatTable(tableView);
    }

signals:

    void OnlyShowAvailablePlayers();

private:

    QTableView* FormatTable(QTableView* tableView)
    {
        // table view
        tableView->verticalHeader()->hide();
        tableView->setStyleSheet(m_style);
        tableView->resizeColumnsToContents();
        tableView->resizeRowsToContents();
        tableView->horizontalHeader()->setStretchLastSection(true);

        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

        return tableView;
    }

    const std::shared_ptr<HitterData>& m_hitterData;
    const std::shared_ptr<PitcherData>& m_pitcherData;

    const QString m_style = 
        R"""(
        QTableView, QHeaderView, QToolTip {
            font-family: "Consolas";
            font-size: 11px;
        })""";
};

std::shared_ptr<HitterData> ParseHitters()
{
    //
    // Hitter data
    //

    std::fstream batters("batters.csv");
    std::string row;

    using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;

    // Tokenize header data
    std::getline(batters,row);
    Tokenizer tokenizer(row);

    // Parse header
    for(auto& lut : streamer_hitters) {

        // find entry key
        auto itr = std::find(tokenizer.begin(),tokenizer.end(),lut.first);
        if(itr != tokenizer.end()) {

            // set value
            lut.second = std::distance(tokenizer.begin(),itr);
        }
    }

    auto spHitterData = std::make_shared<HitterData>();

    while(std::getline(batters,row)) {

        Tokenizer tokenizer(row);
        std::vector<std::string> parsed(tokenizer.begin(),tokenizer.end());

        Hitter be;
        be.name = parsed[streamer_hitters["Name"]];
        be.team = parsed[streamer_hitters["Team"]];
        be.plateApperances = boost::lexical_cast<uint32_t>(parsed[streamer_hitters["PA"]]);
        be.atBats = boost::lexical_cast<uint32_t>(parsed[streamer_hitters["AB"]]);
        be.hits = boost::lexical_cast<uint32_t>(parsed[streamer_hitters["H"]]);
        be.homeRuns = boost::lexical_cast<uint32_t>(parsed[streamer_hitters["HR"]]);
        be.runs = boost::lexical_cast<uint32_t>(parsed[streamer_hitters["R"]]);
        be.rBIs = boost::lexical_cast<uint32_t>(parsed[streamer_hitters["RBI"]]);
        be.stolenBases = boost::lexical_cast<uint32_t>(parsed[streamer_hitters["SB"]]);
        be.average = float(be.hits) / float(be.atBats);

        auto TeamGroup = LookupTeamGroup(be.team);

        if(TeamGroup.Leauge != Leauge::NL) {
            continue;
        }

        if(be.plateApperances < 20) {
            continue;
        }

        spHitterData->vecHitters.emplace_back(be);
    }

    spHitterData->UpdateZScores();

    return spHitterData;
}


std::shared_ptr<PitcherData> ParsePitchers()
{
    //
    // Parse data
    //

    std::fstream pitchers("pitchers.csv");
    std::string row;

    using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;

    // Tokenize header data
    std::getline(pitchers, row);
    Tokenizer tokenizer(row);

    // Parse header
    for(auto& lut : streamer_pitchers) {

        // find entry key
        auto itr = std::find(tokenizer.begin(),tokenizer.end(),lut.first);
        if(itr != tokenizer.end()) {

            // set value
            lut.second = std::distance(tokenizer.begin(),itr);
        }
    }

    auto spPitcherData = std::make_shared<PitcherData>();

    while(std::getline(pitchers,row)) {

        Tokenizer tokenizer(row);
        std::vector<std::string> parsed(tokenizer.begin(),tokenizer.end());

        Pitcher pe;
        pe.name = parsed[streamer_pitchers["Name"]];
        pe.team = parsed[streamer_pitchers["Team"]];
        pe.IP = boost::lexical_cast<decltype(pe.IP)>(parsed[streamer_pitchers["IP"]]);
        pe.ERA = boost::lexical_cast<decltype(pe.ERA)>(parsed[streamer_pitchers["ERA"]]);
        pe.WHIP = boost::lexical_cast<decltype(pe.WHIP)>(parsed[streamer_pitchers["WHIP"]]);
        pe.W = boost::lexical_cast<decltype(pe.W)>(parsed[streamer_pitchers["W"]]);
        pe.SO = boost::lexical_cast<decltype(pe.SO)>(parsed[streamer_pitchers["SO"]]);
        pe.SV = boost::lexical_cast<decltype(pe.SV)>(parsed[streamer_pitchers["SV"]]);

        auto TeamGroup = LookupTeamGroup(pe.team);

        if(TeamGroup.Leauge != Leauge::NL) {
            continue;
        }

        if (pe.IP < 5) {
            continue;
        }

        spPitcherData->vecPitchers.emplace_back(pe);
    }

    spPitcherData->UpdateZScores();

    return spPitcherData;
}

int main(int argc,char *argv[])
{
    
    QApplication app(argc,argv);
    app.setStyle(QStyleFactory::create("Fusion"));

    // QPalette darkPalette;
    // darkPalette.setColor(QPalette::Window,QColor(53,53,53));
    // darkPalette.setColor(QPalette::WindowText,Qt::white);
    // darkPalette.setColor(QPalette::Base,QColor(25,25,25));
    // darkPalette.setColor(QPalette::AlternateBase,QColor(53,53,53));
    // darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
    // darkPalette.setColor(QPalette::ToolTipText,Qt::white);
    // darkPalette.setColor(QPalette::Text,Qt::white);
    // darkPalette.setColor(QPalette::Button,QColor(53,53,53));
    // darkPalette.setColor(QPalette::ButtonText,Qt::white);
    // darkPalette.setColor(QPalette::BrightText,Qt::red);
    // darkPalette.setColor(QPalette::Link,QColor(42,130,218));
    // darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
    // darkPalette.setColor(QPalette::HighlightedText,Qt::black);

    // app.setPalette(darkPalette);
    // app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

    auto spHitterData = ParseHitters();
    auto spPitcherData = ParsePitchers();

    MainWindow mainWin(spHitterData, spPitcherData);
    mainWin.resize(1000, 800);
    mainWin.show();

    return app.exec();
}
