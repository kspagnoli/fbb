#include "PitcherTableModel.h"
#include "Pitcher.h"

#include <vector>
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

//------------------------------------------------------------------------------
// PitcherTableModel
//------------------------------------------------------------------------------
PitcherTableModel::PitcherTableModel(const std::string& filename, QObject* parent)
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
            pitcher.name = parsed[LUT["Name"]];
            pitcher.team = parsed[LUT["Team"]];
            pitcher.IP = boost::lexical_cast<decltype(pitcher.IP)>(parsed[LUT["IP"]]);
            pitcher.ERA = boost::lexical_cast<decltype(pitcher.ERA)>(parsed[LUT["ERA"]]);
            pitcher.WHIP = boost::lexical_cast<decltype(pitcher.WHIP)>(parsed[LUT["WHIP"]]);
            pitcher.W = boost::lexical_cast<decltype(pitcher.W)>(parsed[LUT["W"]]);
            pitcher.SO = boost::lexical_cast<decltype(pitcher.SO)>(parsed[LUT["SO"]]);
            pitcher.SV = boost::lexical_cast<decltype(pitcher.SV)>(parsed[LUT["SV"]]);

            if (pitcher.IP < 5) {
                continue;
            }

            m_vecPitchers.emplace_back(pitcher);

        } catch (...) {

            // Try next if something went wrong
            continue;
        }
    }
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

    if (role == Qt::DisplayRole || role == RawDataRole) {

        switch (index.column())
        {
        case COLUMN_RANK:
            return index.row() + 1;
        case COLUMN_NAME:
            return QString::fromStdString(pitcher.name);
        case COLUMN_TEAM:
            return QString::fromStdString(pitcher.team);
        case COLUMN_POSITION:
            return pitcher.positions;
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
            return QString::fromStdString(pitcher.comment);
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
QVariant PitcherTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
