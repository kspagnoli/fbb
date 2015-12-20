#include "HitterTableModel.h"
#include "Hitter.h"
#include "ZScore.h"
#include "Teams.h"

#include <vector>
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <QAbstractItemDelegate>

//------------------------------------------------------------------------------
// GetOwnerName (TEMP helper)
//------------------------------------------------------------------------------
static QString GetOwnerName(uint32_t ownerId)
{
    switch (ownerId)
    {
    case 0:
        return "--";
    case 1:
        return "Team A";
    case 2:
        return "Team B";
    default:
        return "???";
    }
}

//------------------------------------------------------------------------------
// PositionToString (static helper)
//------------------------------------------------------------------------------
static QString PositionToString(const Hitter::PositionMask& positions)
{
    // Quick return for unknown
    if (positions == uint32_t(Hitter::Position::None)) {
        return "--";
    }

    // Build up list
    QStringList vecPos;
    if (positions & uint32_t(Hitter::Position::Catcher)) { vecPos.push_back("C"); }
    if (positions & uint32_t(Hitter::Position::First)) { vecPos.push_back("1B"); }
    if (positions & uint32_t(Hitter::Position::Second)) { vecPos.push_back("2B"); }
    if (positions & uint32_t(Hitter::Position::SS)) { vecPos.push_back("SS"); }
    if (positions & uint32_t(Hitter::Position::Third)) { vecPos.push_back("3B"); }
    if (positions & uint32_t(Hitter::Position::Outfield)) { vecPos.push_back("OF"); }
    if (positions & uint32_t(Hitter::Position::DH)) { vecPos.push_back("DH"); }
    return vecPos.join(", ");
}

//------------------------------------------------------------------------------
// HitterTableModel
//------------------------------------------------------------------------------
HitterTableModel::HitterTableModel(const std::string& filename, const PlayerApperances& playerApperances, QObject* parent)
    : QAbstractTableModel(parent)
{
    // Open file
    std::fstream batters(filename);
    std::string row;

    // Tokenize header data
    using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;
    std::getline(batters, row);
    Tokenizer tokenizer(row);

    // Stats to find
    std::unordered_map<std::string, uint32_t> LUT =
    {
        { "Name",0 },
        { "Team",0 },
        { "PA",0 },
        { "AB",0 },
        { "H",0 },
        { "HR",0 },
        { "R",0 },
        { "RBI",0 },
        { "SB",0 },
    };

    // Parse the header data (first row)
    for (auto& entry : LUT) {

        // Populate LUT to figure out stat-to-column association
        auto itr = std::find(tokenizer.begin(), tokenizer.end(), entry.first);
        if (itr != tokenizer.end()) {
            entry.second = std::distance(tokenizer.begin(), itr);
        }
    }

    // Loop rows
    while (std::getline(batters, row)) {

        // Tokenization and/or lexical casts might fail if the data is malformed
        try {

            // Tokenize this row
            Tokenizer tokenizer(row);
            std::vector<std::string> parsed(tokenizer.begin(), tokenizer.end());

            // Parse into hitter data
            Hitter hitter;
            hitter.name = parsed[LUT["Name"]];
            hitter.team = parsed[LUT["Team"]];
            hitter.PA = boost::lexical_cast<uint32_t>(parsed[LUT["PA"]]);
            hitter.AB = boost::lexical_cast<uint32_t>(parsed[LUT["AB"]]);
            hitter.H = boost::lexical_cast<uint32_t>(parsed[LUT["H"]]);
            hitter.HR = boost::lexical_cast<uint32_t>(parsed[LUT["HR"]]);
            hitter.R = boost::lexical_cast<uint32_t>(parsed[LUT["R"]]);
            hitter.RBI = boost::lexical_cast<uint32_t>(parsed[LUT["RBI"]]);
            hitter.SB = boost::lexical_cast<uint32_t>(parsed[LUT["SB"]]);
            hitter.AVG = float(hitter.H) / float(hitter.AB);

            // Skip players with little-to-no AB
            if (hitter.AB < 10) {
                continue;
            }

            if (LookupTeamGroup(hitter.team).leauge != Leauge::NL) {
                continue;
            }

            // Lookup appearances 
            try {
                const auto& appearances = playerApperances.Lookup(hitter.name);
                if (appearances.atC > 0) { hitter.positions |= int32_t(Hitter::Position::Catcher); }
                if (appearances.at1B > 0) { hitter.positions |= int32_t(Hitter::Position::First); }
                if (appearances.at2B > 0) { hitter.positions |= int32_t(Hitter::Position::Second); }
                if (appearances.atSS > 0) { hitter.positions |= int32_t(Hitter::Position::SS); }
                if (appearances.at3B > 0) { hitter.positions |= int32_t(Hitter::Position::Third); }
                if (appearances.atOF > 0) { hitter.positions |= int32_t(Hitter::Position::Outfield); }
                if (appearances.atDH > 0) { hitter.positions |= int32_t(Hitter::Position::DH); }
            } catch (std::runtime_error& e) {
                std::cerr << "[Hitter] " << e.what() << std::endl;
            }

            // Store in vector
            m_vecHitters.emplace_back(hitter);

        } catch (...) {

            // Try next if something went wrong
            continue;
        }
    }

    // Calculated zScores
    GET_ZSCORE(m_vecHitters, AVG, zAVG);
    GET_ZSCORE(m_vecHitters, HR, zHR);
    GET_ZSCORE(m_vecHitters, R, zR);
    GET_ZSCORE(m_vecHitters, RBI, zRBI);
    GET_ZSCORE(m_vecHitters, SB, zSB);

    // Calculated weighted zScores
    for (Hitter& hitter : m_vecHitters) {
        hitter.wAVG = hitter.AB * hitter.zAVG;
    }
    GET_ZSCORE(m_vecHitters, wAVG, zAVG);

    // Sum zScores
    for (Hitter& hitter : m_vecHitters) {
        hitter.zScore = (hitter.zAVG + hitter.zHR + hitter.zR + hitter.zRBI + hitter.zSB);
    }

    // Re-rank based on z-score
    std::sort(std::begin(m_vecHitters), std::end(m_vecHitters), [](const auto& lhs, const auto& rhs) {
        return lhs.zScore > rhs.zScore;
    });

    // [TODO] make me a setting
    const size_t HITTER_RATIO = 68;
    const size_t BUDGET = 260;
    const size_t NUM_OWNERS = 12;
    const size_t NUM_HITTERS_PER_OWNER = 14;
    const size_t TOTAL_HITTERS = NUM_HITTERS_PER_OWNER * NUM_OWNERS;
    const size_t TOTAL_HITTER_MONEY = (NUM_OWNERS * BUDGET * HITTER_RATIO) / size_t(100);

    // Get the "replacement player"
    auto zReplacement = m_vecHitters[TOTAL_HITTERS].zScore;

    // Scale all players based off the replacement player
    float sumPositiveZScores = 0;
    std::for_each(std::begin(m_vecHitters), std::end(m_vecHitters), [&](Hitter& hitter) {
        hitter.zScore -= zReplacement;
        if (hitter.zScore > 0) {
            sumPositiveZScores += hitter.zScore;
        }
    });

    // Apply cost ratio
    static const float costPerZ = float(TOTAL_HITTER_MONEY) / sumPositiveZScores;
    std::for_each(std::begin(m_vecHitters), std::end(m_vecHitters), [&](Hitter& hitter) {
        hitter.cost = hitter.zScore * costPerZ;
    });
}

//------------------------------------------------------------------------------
// rowCount (override)
//------------------------------------------------------------------------------
int HitterTableModel::rowCount(const QModelIndex& index) const 
{
    return int(m_vecHitters.size());
}

//------------------------------------------------------------------------------
// columnCount (override)
//------------------------------------------------------------------------------
int HitterTableModel::columnCount(const QModelIndex& index) const 
{
    return COLUMN_COUNT;
}

//------------------------------------------------------------------------------
// data (override)
//------------------------------------------------------------------------------
QVariant HitterTableModel::data(const QModelIndex& index, int role) const
{
    const Hitter& hitter = m_vecHitters.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole || role == RawDataRole) {

        switch (index.column())
        {
        case COLUMN_RANK:
            return index.row() + 1;
        case COLUMN_DRAFT_BUTTON:
            return uint32_t(hitter.status);
        case COLUMN_OWNER:
            if (role == RawDataRole) {
                return hitter.ownerId;
            } else {
                return GetOwnerName(hitter.ownerId);
            }
        case COLUMN_PAID:
            if (role == RawDataRole) {
                return hitter.paid;
            } else {
                return hitter.paid != 0 ? QString("$%1").arg(hitter.paid) : QString("--");
            }
        case COLUMN_DRAFT_POSITION:
            if (role == RawDataRole) {
                return hitter.draftPosition;
            } else {
                return PositionToString(hitter.draftPosition);
            }
        case COLUMN_NAME:
            return QString::fromStdString(hitter.name);
        case COLUMN_TEAM:
            return QString::fromStdString(hitter.team);
        case COLUMN_POSITION:
            if (role == RawDataRole) {
                return hitter.positions;
            } else {
                return PositionToString(hitter.positions);
            }
        case COLUMN_AB:
            return hitter.AB;
        case COLUMN_AVG:
            if (role == RawDataRole) {
                return hitter.AVG;
            } else {
                return QString::number(hitter.AVG, 'f', 3);
            }
        case COLUMN_HR:
            return hitter.HR;
        case COLUMN_R:
            return hitter.R;
        case COLUMN_RBI:
            return hitter.RBI;
        case COLUMN_SB:
            return hitter.SB;
        case COLUMN_ESTIMATE:
            if (role == RawDataRole) {
                return hitter.cost;
            } else {
                return QString("$%1").arg(QString::number(hitter.cost, 'f', 2));
            }
        case COLUMN_Z:
            if (role == RawDataRole) {
                return hitter.zScore;
            } else {
                return QString::number(hitter.zScore, 'f', 3);
            }
        case COLUMN_COMMENT:
            return QString::fromStdString(hitter.comment);
        }
    } 
    
    if (role == Qt::TextAlignmentRole) {

        switch (index.column())
        {
        case COLUMN_PAID:
        case COLUMN_OWNER:
            return Qt::AlignmentFlag(int(Qt::AlignCenter) | int(Qt::AlignVCenter));
        case COLUMN_NAME:
        case COLUMN_TEAM:
        case COLUMN_COMMENT:
            return Qt::AlignmentFlag(int(Qt::AlignLeft) | int(Qt::AlignVCenter));
        default:
            return Qt::AlignmentFlag(int(Qt::AlignRight) | int(Qt::AlignVCenter));
        }
    }
    
    if (role == Qt::ToolTipRole) {

        switch(index.column())
        {
        case COLUMN_Z:
            return QString("zAVG: %1\n"
                           "zR:   %2\n"
                           "zRBI: %3\n"
                           "zHR:  %4\n"
                           "zSB:  %5")
                .arg(hitter.zAVG)
                .arg(hitter.zR)
                .arg(hitter.zRBI)
                .arg(hitter.zHR)
                .arg(hitter.zSB);
        }
    }

    return QVariant();
}

//------------------------------------------------------------------------------
// headerData (override)
//------------------------------------------------------------------------------
QVariant HitterTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {

        if (role == Qt::DisplayRole) {

            switch (section)
            {
            case COLUMN_RANK:
                return "#";
            case COLUMN_DRAFT_BUTTON:
                return "Status";
            case COLUMN_OWNER:
                return "Owner";
            case COLUMN_PAID:
                return "Paid";
            case COLUMN_DRAFT_POSITION:
                return "Position";
            case COLUMN_NAME:
                return "Name";
            case COLUMN_TEAM:
                return "Team";
            case COLUMN_POSITION:
                return "Position";
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

//------------------------------------------------------------------------------
// flags (override)
//------------------------------------------------------------------------------
Qt::ItemFlags HitterTableModel::flags(const QModelIndex &index) const
{
    switch (index.column()) 
    {
    case COLUMN_DRAFT_BUTTON:
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    default:
        return QAbstractItemModel::flags(index);
    }
}

//------------------------------------------------------------------------------
// setData (override)
//------------------------------------------------------------------------------
bool HitterTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    Hitter& hitter = m_vecHitters.at(index.row());

    switch (index.column())
    {
    case COLUMN_DRAFT_BUTTON:
        hitter.status = Player::Status(value.toInt());
        return true;
    case COLUMN_PAID:
        hitter.paid = value.toInt();
        return true;
    case COLUMN_OWNER:
        hitter.ownerId = value.toInt();
        return true;
    default:
        return false;
    }

}
