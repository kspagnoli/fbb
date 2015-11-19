#include "HitterTableModel.h"
#include "Hitter.h"

#include <vector>
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

//------------------------------------------------------------------------------
// HitterTableModel
//------------------------------------------------------------------------------
HitterTableModel::HitterTableModel(const std::string& filename, QObject* parent)
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
    std::unordered_map<std::string, uint32_t> statColumnLUT =
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
    for (auto& lut : statColumnLUT) {

        // Populate LUT to figure out stat-to-column association
        auto itr = std::find(tokenizer.begin(), tokenizer.end(), lut.first);
        if (itr != tokenizer.end()) {
            lut.second = std::distance(tokenizer.begin(), itr);
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
            hitter.name = parsed[statColumnLUT["Name"]];
            hitter.team = parsed[statColumnLUT["Team"]];
            hitter.PA = boost::lexical_cast<uint32_t>(parsed[statColumnLUT["PA"]]);
            hitter.AB = boost::lexical_cast<uint32_t>(parsed[statColumnLUT["AB"]]);
            hitter.H = boost::lexical_cast<uint32_t>(parsed[statColumnLUT["H"]]);
            hitter.HR = boost::lexical_cast<uint32_t>(parsed[statColumnLUT["HR"]]);
            hitter.R = boost::lexical_cast<uint32_t>(parsed[statColumnLUT["R"]]);
            hitter.RBI = boost::lexical_cast<uint32_t>(parsed[statColumnLUT["RBI"]]);
            hitter.SB = boost::lexical_cast<uint32_t>(parsed[statColumnLUT["SB"]]);
            hitter.AVG = float(hitter.H) / float(hitter.AB);

            // Store in vector
            m_vecHitters.emplace_back(hitter);

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

    if (role == Qt::DisplayRole) {

        switch (index.column())
        {
        case COLUMN_RANK:
            return index.row() + 1;
        case COLUMN_NAME:
            return QString::fromStdString(hitter.name);
        case COLUMN_TEAM:
            return QString::fromStdString(hitter.team);
        case COLUMN_POSITION:
            return "POS";
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
