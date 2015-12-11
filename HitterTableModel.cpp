#include "HitterTableModel.h"
#include "Hitter.h"

#include <vector>
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>

//------------------------------------------------------------------------------
// PositionToString (static helper)
//------------------------------------------------------------------------------
static QString PositionToString(const Hitter::PositionMask& positions)
{
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

            // Lookup appearances 
            const auto& appearances = playerApperances.Lookup(hitter.name);

            // Parse positions
            if (appearances.atC  >= 20) { hitter.positions |= int32_t(Hitter::Position::Catcher); }
            if (appearances.at1B >= 20) { hitter.positions |= int32_t(Hitter::Position::First); }
            if (appearances.at2B >= 20) { hitter.positions |= int32_t(Hitter::Position::Second); }
            if (appearances.atSS >= 20) { hitter.positions |= int32_t(Hitter::Position::SS); }
            if (appearances.at3B >= 20) { hitter.positions |= int32_t(Hitter::Position::Third); }
            if (appearances.atOF >= 20) { hitter.positions |= int32_t(Hitter::Position::Outfield); }
            if (appearances.atDH >= 20) { hitter.positions |= int32_t(Hitter::Position::DH); }

            // Store in vector
            m_vecHitters.emplace_back(hitter);

        } catch (std::runtime_error& e) {

            std::cout << e.what() << std::endl;

        } catch (...) {

            // Try next if something went wrong
            continue;
        }
    }
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

    if (role == Qt::DisplayRole || role == RawDataRole) {

        switch (index.column())
        {
        case COLUMN_RANK:
            return index.row() + 1;
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
            return hitter.cost;
        case COLUMN_Z:
            return hitter.zScore;
        case COLUMN_COMMENT:
            return QString::fromStdString(hitter.comment);
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
