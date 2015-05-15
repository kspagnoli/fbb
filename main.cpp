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

struct TeamGroup
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

struct DraftablePlayer
{
    // flags
    static const int32_t Undrafted = -1;

    // 
    enum class PlayerStatus 
    {
        Active, 
        Injured, 
        Minors ,
    };

    // player data
    std::string name;
    std::string team;

    // 
    PlayerStatus status = PlayerStatus::Active;
    
    // fantasy
    int32_t ownerId = Undrafted;
    float paid = 0;

    // estimate
    float zScore = 0;
    float cost = 0;

    // comment
    std::string comment;
};

struct Hitter : public DraftablePlayer
{
    // positions
    std::bitset<HittingPositions::Count> positions;

    // sub-stat
    uint32_t PA = 0;
    uint32_t AB = 0;
    uint32_t H = 0;

    // core stats
    float AVG = 0;
    uint32_t R = 0;
    uint32_t RBI = 0;
    uint32_t HR = 0;
    uint32_t SB = 0;

    // zScores
    float zAVG = 0;
    float zR = 0;
    float zHR = 0;
    float zRBI = 0;
    float zSB = 0;
};

struct Pitcher : public DraftablePlayer
{
    // positions
    std::bitset<PitchingPositions::Count> positions;

    // sub-stats
    float IP = 0;
    uint32_t SO = 0;
    uint32_t W = 0;
    uint32_t SV = 0;
    float ERA = 0;
    float WHIP = 0;

    // zScores
    float zSO = 0;
    float zW = 0;
    float zSV = 0;
    float zERA = 0;
    float zWHIP = 0;
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

static TeamGroup LookupTeamGroup(const std::string& teamName)
{
    static std::unordered_map<std::string, TeamGroup> s_LUT = {

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

// HitterDataModel
class HitterDataModel : public QWidget
{

public:

    // HitterData
    HitterDataModel(const std::string csvFile, QWidget* parent)
        : QWidget(parent)
    {
        // open file
        std::fstream batters(csvFile);
        std::string row;

        using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;

        // tokenize header data
        std::getline(batters,row);
        Tokenizer tokenizer(row);

        // stats to find
        std::unordered_map<std::string,uint32_t> stat_lut =
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

        // Parse header
        for (auto& lut : stat_lut) {

            // find entry key
            auto itr = std::find(tokenizer.begin(),tokenizer.end(),lut.first);
            if(itr != tokenizer.end()) {

                // set value
                lut.second = std::distance(tokenizer.begin(),itr);
            }
        }

        while (std::getline(batters,row)) {

            Tokenizer tokenizer(row);
            std::vector<std::string> parsed(tokenizer.begin(),tokenizer.end());

            Hitter hitter;
            hitter.name = parsed[stat_lut["Name"]];
            hitter.team = parsed[stat_lut["Team"]];
            hitter.PA = boost::lexical_cast<uint32_t>(parsed[stat_lut["PA"]]);
            hitter.AB = boost::lexical_cast<uint32_t>(parsed[stat_lut["AB"]]);
            hitter.H = boost::lexical_cast<uint32_t>(parsed[stat_lut["H"]]);
            hitter.HR = boost::lexical_cast<uint32_t>(parsed[stat_lut["HR"]]);
            hitter.R = boost::lexical_cast<uint32_t>(parsed[stat_lut["R"]]);
            hitter.RBI = boost::lexical_cast<uint32_t>(parsed[stat_lut["RBI"]]);
            hitter.SB = boost::lexical_cast<uint32_t>(parsed[stat_lut["SB"]]);
            hitter.AVG = float(hitter.H) / float(hitter.AB);

            auto TeamGroup = LookupTeamGroup(hitter.team);

            // LEAUGE FILTER
            if (TeamGroup.Leauge != Leauge::NL) {
                continue;
            }

            // [XXX] HAX
            if (hitter.AB < 20) {
                continue;
            }

            // [XXX] HAX
            if (hitter.AB < 100) {
                hitter.status = DraftablePlayer::PlayerStatus::Minors;
            }

            m_vecHitters.emplace_back(hitter);
        }

        InializeZScores();
    }

    // GetHitter
    const Hitter& GetHitter(size_t i) const
    {
        return m_vecHitters[i];
    }

    // Count
    size_t Count() const 
    {
        return m_vecHitters.size();
    }

    

private:

    // UpdateZScores
    void InializeZScores()
    {
        // Helpers
        const auto fnGetAB  = [](const Hitter& hitter) { return hitter.AB; };
        const auto fnGetAVG = [](const Hitter& hitter) { return hitter.AVG; };
        const auto fnGetHR  = [](const Hitter& hitter) { return hitter.HR; };
        const auto fnGetR   = [](const Hitter& hitter) { return hitter.R; };
        const auto fnGetRBI = [](const Hitter& hitter) { return hitter.RBI; };
        const auto fnGetSB  = [](const Hitter& hitter) { return hitter.SB; };

        // Averages
        const float avgAB  = average(fnGetAB,m_vecHitters);
        const float avgAVG = average(fnGetAVG,m_vecHitters);
        const float avgHR  = average(fnGetHR,m_vecHitters);
        const float avgR   = average(fnGetR,m_vecHitters);
        const float avgRBI = average(fnGetRBI,m_vecHitters);
        const float avgSB  = average(fnGetSB,m_vecHitters);

        // Standard devs
        const float stdevAB  = stdev(fnGetAB,avgAB,m_vecHitters);
        const float stdevAVG = stdev(fnGetAVG,avgAVG,m_vecHitters);
        const float stdevHR  = stdev(fnGetHR,avgHR,m_vecHitters);
        const float stdevR   = stdev(fnGetR,avgR,m_vecHitters);
        const float stdevRBI = stdev(fnGetRBI,avgRBI,m_vecHitters);
        const float stdevSB  = stdev(fnGetSB,avgSB,m_vecHitters);

        // zscore
        std::for_each(std::begin(m_vecHitters),std::end(m_vecHitters),[&](Hitter& hitter) {

            // per-state zscores
            hitter.zAVG = (hitter.AB * score(hitter.AVG,avgAVG,stdevAVG) - stdevAB) / avgAB;
            hitter.zHR = score(hitter.HR,avgHR,stdevHR);
            hitter.zR = score(hitter.R,avgR,stdevR);
            hitter.zRBI = score(hitter.RBI,avgRBI,stdevRBI);
            hitter.zSB = score(hitter.SB,avgSB,stdevSB);

            // zscore summation
            hitter.zScore = hitter.zAVG + hitter.zHR + hitter.zR + hitter.zRBI + hitter.zSB;
        });

        // Sort by zScore
        std::sort(m_vecHitters.begin(),m_vecHitters.end(),[](const Hitter& lhs,const Hitter& rhs) {
            return lhs.zScore > rhs.zScore;
        });

        // Get the "replacement player"
        float zRplacement = m_vecHitters[Settings::TotalHittersRostered].zScore;

        // Scale all players based off the replacement player
        std::for_each(std::begin(m_vecHitters),std::end(m_vecHitters),[&](Hitter& hitter) {
            hitter.zScore -= zRplacement;
        });

        // Sum all positive zScores
        float sumPositiveZScores = 0;
        std::for_each(std::begin(m_vecHitters),std::end(m_vecHitters),[&](Hitter& hitter) {
            if(hitter.zScore > 0) {
                sumPositiveZScores += hitter.zScore;
            }
        });

        // Apply cost ratio
        static const float costRatio = (Settings::HitterMoney() / 14.f) * (Settings::TotalHittersRostered / sumPositiveZScores);
        std::for_each(std::begin(m_vecHitters),std::end(m_vecHitters),[&](Hitter& hitter) {
            hitter.cost = hitter.zScore * costRatio;
        });
    }

    std::vector<Hitter> m_vecHitters;

};

// PitcherDataModel
class PitcherDataModel : QWidget
{

public:

    PitcherDataModel(const std::string& csvFile, QWidget* parent)
        : QWidget(parent)
    {
        std::fstream pitchers(csvFile);
        std::string row;

        using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;

        // Tokenize header data
        std::getline(pitchers,row);
        Tokenizer tokenizer(row);

        std::unordered_map<std::string,uint32_t> stat_lut =
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

        // Parse header
        for (auto& lut : stat_lut) {

            // find entry key
            auto itr = std::find(tokenizer.begin(),tokenizer.end(),lut.first);
            if (itr != tokenizer.end()) {

                // set value
                lut.second = std::distance(tokenizer.begin(),itr);
            }
        }

        while (std::getline(pitchers,row)) {

            Tokenizer tokenizer(row);
            std::vector<std::string> parsed(tokenizer.begin(),tokenizer.end());

            Pitcher pitcher;
            pitcher.name = parsed[stat_lut["Name"]];
            pitcher.team = parsed[stat_lut["Team"]];
            pitcher.IP = boost::lexical_cast<decltype(pitcher.IP)>(parsed[stat_lut["IP"]]);
            pitcher.ERA = boost::lexical_cast<decltype(pitcher.ERA)>(parsed[stat_lut["ERA"]]);
            pitcher.WHIP = boost::lexical_cast<decltype(pitcher.WHIP)>(parsed[stat_lut["WHIP"]]);
            pitcher.W = boost::lexical_cast<decltype(pitcher.W)>(parsed[stat_lut["W"]]);
            pitcher.SO = boost::lexical_cast<decltype(pitcher.SO)>(parsed[stat_lut["SO"]]);
            pitcher.SV = boost::lexical_cast<decltype(pitcher.SV)>(parsed[stat_lut["SV"]]);

            auto TeamGroup = LookupTeamGroup(pitcher.team);

            if(TeamGroup.Leauge != Leauge::NL) {
                continue;
            }

            if(pitcher.IP < 5) {
                continue;
            }

            m_vecPitchers.emplace_back(pitcher);
        }

        UpdateZScores();
    }

    // GetHitter
    const Pitcher& GetPitcher(size_t i) const
    {
        return m_vecPitchers[i];
    }

    // Count
    size_t Count() const
    {
        return m_vecPitchers.size();
    }

    // UpdateZScores
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
        const float avgIP   = average(fnGetIP,   m_vecPitchers);
        const float avgSO   = average(fnGetSO,   m_vecPitchers);
        const float avgW    = average(fnGetW,    m_vecPitchers);
        const float avgSV   = average(fnGetSV,   m_vecPitchers);
        const float avgERA  = average(fnGetERA,  m_vecPitchers);
        const float avgWHIP = average(fnGetWHIP, m_vecPitchers);

        // Standard devs
        const float stdevIP   = stdev(fnGetIP,   avgIP,   m_vecPitchers);
        const float stdevSO   = stdev(fnGetSO,   avgSO,   m_vecPitchers);
        const float stdevW    = stdev(fnGetW,    avgW,    m_vecPitchers);
        const float stdevSV   = stdev(fnGetSV,   avgSV,   m_vecPitchers);
        const float stdevERA  = stdev(fnGetERA,  avgERA,  m_vecPitchers);
        const float stdevWHIP = stdev(fnGetWHIP, avgWHIP, m_vecPitchers);
        
        // zscore
        std::for_each(std::begin(m_vecPitchers), std::end(m_vecPitchers), [&](Pitcher& pitcher) {

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
        std::sort(m_vecPitchers.begin(), m_vecPitchers.end(), [](const Pitcher& lhs,const Pitcher& rhs) {
            return lhs.zScore > rhs.zScore;
        });

        // Get the "replacement player"
        float zRplacement = m_vecPitchers[Settings::TotalPitchersRostered].zScore;

        // Scale all players based off the replacement player
        std::for_each(std::begin(m_vecPitchers), std::end(m_vecPitchers), [&](Pitcher& pitcher) {
            pitcher.zScore -= zRplacement;
        });

        // Sum all positive zScores
        float sumPositiveZScores = 0;
        std::for_each(std::begin(m_vecPitchers), std::end(m_vecPitchers), [&](Pitcher& pitcher) {
            if (pitcher.zScore > 0) {
                sumPositiveZScores += pitcher.zScore;
            }
        });

        // Apply cost ratio
        static const float costRatio = (Settings::PitcherMoney() / 10.f) * (Settings::TotalPitchersRostered / sumPositiveZScores);
        std::for_each(std::begin(m_vecPitchers), std::end(m_vecPitchers), [&](Pitcher& pitcher) {
            pitcher.cost = pitcher.zScore * costRatio;
        });
    }


private:

    // xxx
    std::vector<Pitcher> m_vecPitchers;

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
    HitterTableModel(const HitterDataModel* hitterDataModel)
        : m_pHitterDataModel(hitterDataModel)
    {
    }

    //
    int rowCount(const QModelIndex &) const override
    {
        return m_pHitterDataModel->Count();
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
        const Hitter& hitter = m_pHitterDataModel->GetHitter(index.row());

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
                return hitter.AB;
            case COLUMN_AVG:
                return QString::number(hitter.AVG, 'f', 3);
            case COLUMN_HR:
                return hitter.HR;
            case COLUMN_R:
                return hitter.R;
            case COLUMN_RBI:
                return hitter.RBI;
            case COLUMN_SB:
                return hitter.SB;
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
                    .arg(hitter.zAVG)
                    .arg(hitter.zR)
                    .arg(hitter.zRBI)
                    .arg(hitter.zHR)
                    .arg(hitter.zSB);
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

    // Hitter data access
    const HitterDataModel* GetHitterData() const
    {
        return m_pHitterDataModel;
    }

private:

    // Hitter data model
    const HitterDataModel* m_pHitterDataModel;
};

class PitcherTableModel : public QAbstractTableModel
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
    PitcherTableModel(const PitcherDataModel* pPitcherData)
        : m_pPitcherDataModel(pPitcherData)
    {
    }

    //
    int rowCount(const QModelIndex &) const override
    {
        return m_pPitcherDataModel->Count();
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
        const Pitcher& pitcher = m_pPitcherDataModel->GetPitcher(index.row());

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
    const PitcherDataModel* GetPitcherData() const
    {
        return m_pPitcherDataModel;
    }

private:

    const PitcherDataModel* m_pPitcherDataModel;
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
        const Hitter& leftHitter  = m_hittleTableModel->GetHitterData()->GetHitter(left.row());
        const Hitter& rightHitter = m_hittleTableModel->GetHitterData()->GetHitter(right.row());

        QVariant leftData = sourceModel()->data(left);
        QVariant rightData = sourceModel()->data(right);

        switch (column)
        {
        case HitterTableModel::COLUMN_AVG:
            return leftHitter.AVG < rightHitter.AVG;
        case HitterTableModel::COLUMN_Z:
            return leftHitter.zScore < rightHitter.zScore;
        case HitterTableModel::COLUMN_ESTIMATE:
            return leftHitter.cost < rightHitter.cost;
        }

        return leftData < rightData;
    }

    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override
    {
        const Hitter& hitter = m_hittleTableModel->GetHitterData()->GetHitter(sourceRow);

        // filter out inactive players
        if (m_filterActive && hitter.status != DraftablePlayer::PlayerStatus::Active) {
            return false;
        }

        return true;
    }


public slots:

    void OnFilterAvailable(bool checked)
    {
        m_filterActive = checked;
        QSortFilterProxyModel::filterChanged();
    }

private:

    HitterTableModel* m_hittleTableModel;

    // filters
    bool m_filterActive = false;
};


// 
class PitcherSortFilterProxyModel : public QSortFilterProxyModel
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
        if (column != right.column()) {
            return false;
        }

        const Pitcher& leftPitcher  = m_pitcherTableModel->GetPitcherData()->GetPitcher(left.row());
        const Pitcher& rightPitcher = m_pitcherTableModel->GetPitcherData()->GetPitcher(right.row());

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

    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override
    {
        const Pitcher& pitcher = m_pitcherTableModel->GetPitcherData()->GetPitcher(sourceRow);

        // filter out inactive players
        if(m_filterActive && pitcher.status != DraftablePlayer::PlayerStatus::Active) {
            return false;
        }

        return true;
    }

public slots:

    void OnFilterAvailable(bool checked)
    {
        m_filterActive = checked;
        QSortFilterProxyModel::filterChanged();
    }

private:

    bool m_filterActive = false;
    PitcherTableModel* m_pitcherTableModel;
};


//
class MainWindow : public QMainWindow
{

public:

    MainWindow()
    {
        QWidget* central = new QWidget();

        // pitcher table model
        HitterDataModel* hitterDataModel = new HitterDataModel("batters.csv", this);
        HitterTableModel* hittleTableModel = new HitterTableModel(hitterDataModel);
        HitterSortFilterProxyModel* hitterSortFilterProxyModel = new HitterSortFilterProxyModel(hittleTableModel);
        QTableView* hitterTableView = MakeTableView(hitterSortFilterProxyModel, HitterTableModel::COLUMN_RANK);

        // pitcher table
        PitcherDataModel* pitcherDataModel = new PitcherDataModel("pitchers.csv", this);
        PitcherTableModel* pitcherTableModel = new PitcherTableModel(pitcherDataModel);
        PitcherSortFilterProxyModel* pitcherSortFilterProxyModel = new PitcherSortFilterProxyModel(pitcherTableModel);
        QTableView* pitcherTableView = MakeTableView(pitcherSortFilterProxyModel, PitcherTableModel::COLUMN_RANK);
       
        // layout
        QVBoxLayout* vBoxLayout = new QVBoxLayout();
        
        // hitter/pitcher tab View
        QTabWidget* tabs = new QTabWidget(this);
        tabs->addTab(hitterTableView, "Hitters");
        tabs->addTab(pitcherTableView, "Pitchers");
        vBoxLayout->addWidget(tabs);

        // filter action
        QAction* filterAvailableAction = new QAction(this);
        filterAvailableAction->setText(tr("Filter Available"));
        filterAvailableAction->setCheckable(true);
        connect(filterAvailableAction, &QAction::triggered, hitterSortFilterProxyModel, &HitterSortFilterProxyModel::OnFilterAvailable);
        connect(filterAvailableAction, &QAction::triggered, pitcherSortFilterProxyModel, &PitcherSortFilterProxyModel::OnFilterAvailable);

        // main toolbar
        QToolBar* toolbar = new QToolBar("Toolbar");
        toolbar->addAction(filterAvailableAction);
        toolbar->setFloatable(false);
        toolbar->setMovable(false);
        QMainWindow::addToolBar(toolbar);

        // set as main window
        QMainWindow::setCentralWidget(central);
        central->setLayout(vBoxLayout);

        // create menu bar
        QMenuBar* menuBar = new QMenuBar();
        menuBar->addAction("MENU BAR");
        QMainWindow::setMenuBar(menuBar);

        // show me
        QMainWindow::show();
    }

private:

    //
    QTableView* MakeTableView(QSortFilterProxyModel* sortFilterProxyModel, int sortColumn)
    {
        QTableView* tableView = new QTableView();
        tableView->setModel(sortFilterProxyModel);
        tableView->setSortingEnabled(true);
        tableView->verticalHeader()->hide();
        tableView->setStyleSheet(m_style);
        tableView->resizeColumnsToContents();
        tableView->setAlternatingRowColors(true);
        tableView->verticalHeader()->setDefaultSectionSize(15);
        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        sortFilterProxyModel->sort(sortColumn);
        return tableView;
    }

    const QString m_style = 
        R"""(
        QTableView, QHeaderView, QToolTip {
            font-family: "Consolas";
            font-size: 11px;
        })""";
};

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

    MainWindow mainWin;
    mainWin.resize(1000, 800);
    mainWin.show();

    return app.exec();
}
