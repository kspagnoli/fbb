#include "PitcherTableModel.h"
#include "Pitcher.h"
#include "PlayerAppearances.h"
#include "ZScore.h"

#include <vector>
#include <fstream>
#include <bitset>
#include <iostream>
#include <unordered_map>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

//------------------------------------------------------------------------------
// PositionToString (static helper)
//------------------------------------------------------------------------------
static QString PositionToString(const Pitcher::PositionMask& positions)
{
    QStringList vecPos;
    if (positions & uint32_t(Pitcher::Position::Starter)) { vecPos.push_back("SP"); }
    if (positions & uint32_t(Pitcher::Position::Relief)) { vecPos.push_back("RP"); }
    return vecPos.join(", ");
}

//------------------------------------------------------------------------------
// PitcherTableModel
//------------------------------------------------------------------------------
PitcherTableModel::PitcherTableModel(const std::string& filename, const PlayerApperances& playerApperances, QObject* parent)
    : QAbstractTableModel(parent)
{
    std::fstream pitchers(filename);
    std::string row;

    using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;

    // Tokenize header data
    std::getline(pitchers, row);
    Tokenizer tokenizer(row);

    std::unordered_map<std::string, uint32_t> LUT =
    {
        { "Name",0 },
        { "Team",0 },
        { "IP",0 },
        { "SO",0 },
        { "ERA",0 },
        { "WHIP",0 },
        { "W",0 },
        { "SV",0 },
    };

    // Parse header
    for (auto& entry : LUT) {

        // find entry key
        auto itr = std::find(tokenizer.begin(), tokenizer.end(), entry.first);
        if (itr != tokenizer.end()) {

            // set value
            entry.second = std::distance(tokenizer.begin(), itr);
        }
    }

    // Loop rows
    while (std::getline(pitchers, row)) {

        // Tokenization and/or lexical casts might fail if the data is malformed
        try {

            Tokenizer tokenizer(row);
            std::vector<std::string> parsed(tokenizer.begin(), tokenizer.end());

            Pitcher pitcher;
            pitcher.name = QString::fromStdString(parsed[LUT["Name"]]);
            pitcher.team = QString::fromStdString(parsed[LUT["Team"]]);
            pitcher.IP = boost::lexical_cast<decltype(pitcher.IP)>(parsed[LUT["IP"]]);
            pitcher.ERA = boost::lexical_cast<decltype(pitcher.ERA)>(parsed[LUT["ERA"]]);
            pitcher.WHIP = boost::lexical_cast<decltype(pitcher.WHIP)>(parsed[LUT["WHIP"]]);
            pitcher.W = boost::lexical_cast<decltype(pitcher.W)>(parsed[LUT["W"]]);
            pitcher.SO = boost::lexical_cast<decltype(pitcher.SO)>(parsed[LUT["SO"]]);
            pitcher.SV = boost::lexical_cast<decltype(pitcher.SV)>(parsed[LUT["SV"]]);

            if (pitcher.IP < 5) {
                continue;
            }

            // Lookup appearances 
            const auto& appearances = playerApperances.Lookup(pitcher.name.toStdString());
            if (float(appearances.G) * 0.9f < float(appearances.GS)) { 
                pitcher.positions |= int32_t(Pitcher::Position::Starter); 
            } else {
                pitcher.positions |= int32_t(Pitcher::Position::Relief);
            }

            m_vecPitchers.emplace_back(pitcher);

        } catch (std::runtime_error& e) {

            std::cerr << "[Pitcher] " << e.what() << std::endl;

        } catch (...) {

            // Try next if something went wrong
            continue;
        }
    }

    // Calculated zScores
    GET_ZSCORE(m_vecPitchers, SO, zSO);
    GET_ZSCORE(m_vecPitchers, W, zW);
    GET_ZSCORE(m_vecPitchers, SV, zSV);
    GET_ZSCORE(m_vecPitchers, ERA, zERA);
    GET_ZSCORE(m_vecPitchers, WHIP, zWHIP);

    // Calculated weighted zScores
    for (Pitcher& pitcher : m_vecPitchers) {
        pitcher.wERA = pitcher.IP * pitcher.zERA;
        pitcher.wWHIP = pitcher.IP * pitcher.zWHIP;
    }
    GET_ZSCORE(m_vecPitchers, wERA, zERA);
    GET_ZSCORE(m_vecPitchers, wWHIP, zWHIP);

    // Invert ERA and WHIP... lower is better
    for (Pitcher& pitcher : m_vecPitchers) {
        pitcher.zERA =  -pitcher.zERA;
        pitcher.zWHIP = -pitcher.zWHIP;
    }

    // Sum zscore
    for (Pitcher& pitcher : m_vecPitchers) {
        pitcher.zScore = (pitcher.zSO + pitcher.zW + pitcher.zSV + pitcher.zERA + pitcher.zWHIP);
    }

    // Re-rank based on z-score
    std::sort(m_vecPitchers.begin(), m_vecPitchers.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.zScore > rhs.zScore;
    });

    const size_t HITTER_RATIO = 68;
    const size_t BUDGET = 260;
    const size_t NUM_OWNERS = 12;
    const size_t NUM_PITCHERS_PER_OWNER = 10;
    const size_t TOTAL_PITCHERS = NUM_PITCHERS_PER_OWNER * NUM_OWNERS;
    const size_t TOTAL_PITCHER_MONEY = (NUM_OWNERS * BUDGET * (100-HITTER_RATIO)) / size_t(100);

    // Get the "replacement player"
    auto zReplacement = m_vecPitchers[TOTAL_PITCHERS].zScore;

    // Scale all players based off the replacement player
    float sumPositiveZScores = 0;
    std::for_each(std::begin(m_vecPitchers), std::end(m_vecPitchers), [&](Pitcher& pitcher) {
        pitcher.zScore -= zReplacement;
        if (pitcher.zScore > 0) {
            sumPositiveZScores += pitcher.zScore;
        }
    });

    // Apply cost ratio
    static const float costPerZ = float(TOTAL_PITCHER_MONEY) / sumPositiveZScores;
    std::for_each(std::begin(m_vecPitchers), std::end(m_vecPitchers), [&](Pitcher& pitcher) {
        pitcher.cost = pitcher.zScore * costPerZ;
    });
}

//------------------------------------------------------------------------------
// rowCount (override)
//------------------------------------------------------------------------------
int PitcherTableModel::rowCount(const QModelIndex& index) const
{
    return int(m_vecPitchers.size());
}

//------------------------------------------------------------------------------
// columnCount (override)
//------------------------------------------------------------------------------
int PitcherTableModel::columnCount(const QModelIndex& index) const
{
    return COLUMN_COUNT;
}

//------------------------------------------------------------------------------
// data (override)
//------------------------------------------------------------------------------
QVariant PitcherTableModel::data(const QModelIndex& index, int role) const
{
    const Pitcher& pitcher = m_vecPitchers.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole || role == RawDataRole) {

        switch (index.column())
        {
        case COLUMN_RANK:
            return index.row() + 1;
        case COLUMN_DRAFT_STATUS:
            return uint32_t(pitcher.status);
        case COLUMN_NAME:
            return pitcher.name;
        case COLUMN_TEAM:
            return pitcher.team;
        case COLUMN_POSITION:
            if (role == RawDataRole) {
                return pitcher.positions;
            } else {
                return PositionToString(pitcher.positions);
            }
        case COLUMN_IP:
            if (role == RawDataRole) {
                return pitcher.IP;
            } else {
                QString::number(pitcher.IP, 'f', 1);
            }
        case COLUMN_SO:
            return pitcher.SO;
        case COLUMN_ERA:
            if (role == RawDataRole) {
                return pitcher.ERA;
            } else {
                return QString::number(pitcher.ERA, 'f', 3);
            }
        case COLUMN_WHIP:
            if (role == RawDataRole) {
                return pitcher.WHIP;
            } else {
                return QString::number(pitcher.WHIP, 'f', 3);
            }
        case COLUMN_W:
            return pitcher.W;
        case COLUMN_SV:
            return pitcher.SV;
        case COLUMN_Z:
            if (role == RawDataRole) {
                return pitcher.zScore;
            } else {
                return QString::number(pitcher.zScore, 'f', 3);
            }
        case COLUMN_ESTIMATE:
            if (role == RawDataRole) {
                return pitcher.cost;
            } else {
                return QString("$%1").arg(QString::number(pitcher.cost, 'f', 2));
            }
        case COLUMN_COMMENT:
            return pitcher.comment;
        }
    }

    if (role == Qt::TextAlignmentRole) {

        switch (index.column())
        {
        case COLUMN_NAME:
        case COLUMN_TEAM:
        case COLUMN_COMMENT:
            return Qt::AlignmentFlag(int(Qt::AlignLeft) | int(Qt::AlignVCenter));
        default:
            return Qt::AlignmentFlag(int(Qt::AlignRight) | int(Qt::AlignVCenter));
        }
    }

    if (role == Qt::ToolTipRole) {

        switch (index.column())
        {
        case COLUMN_Z:
            return QString("zERA:  %1\n"
                           "zSO:   %2\n"
                           "zWHIP: %3\n"
                           "zW:    %4\n"
                           "zSV:   %5")
                .arg(pitcher.zERA)
                .arg(pitcher.zSO)
                .arg(pitcher.zWHIP)
                .arg(pitcher.zW)
                .arg(pitcher.zSV);
        }
    }

    return QVariant();
}

//------------------------------------------------------------------------------
// headerData (override)
//------------------------------------------------------------------------------
QVariant PitcherTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {

        if (role == Qt::DisplayRole) {

            switch (section)
            {
            case COLUMN_RANK:
                return "#";
            case COLUMN_DRAFT_STATUS:
                return "Status";
            case COLUMN_NAME:
                return "Name";
            case COLUMN_TEAM:
                return "Team";
            case COLUMN_CATERGORY:
                return "Catergory";
            case COLUMN_POSITION:
                return "Position";
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

Qt::ItemFlags PitcherTableModel::flags(const QModelIndex& index) const
{
    switch (index.column())
    {
    case COLUMN_DRAFT_STATUS:
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    default:
        return QAbstractItemModel::flags(index);
    }
}

bool PitcherTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Pitcher& pitcher = m_vecPitchers.at(index.row());

    switch (index.column())
    {
    case COLUMN_DRAFT_STATUS:
        pitcher.status = Player::Status(value.toInt());
        return true;
    default:
        return false;
    }
}
