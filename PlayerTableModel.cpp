#include "PlayerTableModel.h"
#include "ZScore.h"
#include "Teams.h"
#include "Player.h"

#include <vector>
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <QAbstractItemDelegate>

// [XXX] make me settings
static const size_t HITTER_RATIO = 68;
static const size_t BUDGET = 260;
static const size_t NUM_OWNERS = 12;
static const size_t NUM_HITTERS_PER_OWNER = 14;
static const size_t NUM_PITCHERS_PER_OWNER = 10;
static const size_t TOTAL_HITTERS = NUM_HITTERS_PER_OWNER * NUM_OWNERS;
static const size_t TOTAL_HITTER_MONEY = (NUM_OWNERS * BUDGET * HITTER_RATIO) / size_t(100);
static const size_t TOTAL_PITCHERS = NUM_PITCHERS_PER_OWNER * NUM_OWNERS;
static const size_t TOTAL_PITCHER_MONEY = (NUM_OWNERS * BUDGET * (100 - HITTER_RATIO)) / size_t(100);

//------------------------------------------------------------------------------
// GetOwnerName
// [XXX] Temp!
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
static QString PositionToString(const Player::PositionMask& positions)
{
    // Quick return for unknown
    if (positions == uint32_t(Player::None)) {
        return "--";
    }

    // Build up list
    QStringList vecPos;
    if (positions & Player::Catcher) { vecPos.push_back("C"); }
    if (positions & Player::First) { vecPos.push_back("1B"); }
    if (positions & Player::Second) { vecPos.push_back("2B"); }
    if (positions & Player::SS) { vecPos.push_back("SS"); }
    if (positions & Player::Third) { vecPos.push_back("3B"); }
    if (positions & Player::Outfield) { vecPos.push_back("OF"); }
    if (positions & Player::DH) { vecPos.push_back("DH"); }
    return vecPos.join(", ");
}

//------------------------------------------------------------------------------
// PlayerTableModel
//------------------------------------------------------------------------------
PlayerTableModel::PlayerTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

//------------------------------------------------------------------------------
// PlayerTableModel
//------------------------------------------------------------------------------
void PlayerTableModel::LoadHittingProjections(const std::string& filename, const PlayerApperances& playerApperances)
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
            Player player;
            player.name = QString::fromStdString(parsed[LUT["Name"]]);
            player.team = QString::fromStdString(parsed[LUT["Team"]]);
            player.hitting.PA = boost::lexical_cast<uint32_t>(parsed[LUT["PA"]]);
            player.hitting.AB = boost::lexical_cast<uint32_t>(parsed[LUT["AB"]]);
            player.hitting.H = boost::lexical_cast<uint32_t>(parsed[LUT["H"]]);
            player.hitting.HR = boost::lexical_cast<uint32_t>(parsed[LUT["HR"]]);
            player.hitting.R = boost::lexical_cast<uint32_t>(parsed[LUT["R"]]);
            player.hitting.RBI = boost::lexical_cast<uint32_t>(parsed[LUT["RBI"]]);
            player.hitting.SB = boost::lexical_cast<uint32_t>(parsed[LUT["SB"]]);
            player.hitting.AVG = float(player.hitting.H) / float(player.hitting.AB);

            // Skip players with little-to-no AB
            if (player.hitting.AB < 10) {
                continue;
            }

            if (LookupTeamGroup(player.team.toStdString()).leauge != Leauge::NL) {
                continue;
            }

            // Lookup appearances 
            try {
                const auto& appearances = playerApperances.Lookup(player.name.toStdString());
                if (appearances.atC > 0) { player.eligiblePositions |=  Player::Catcher; }
                if (appearances.at1B > 0) { player.eligiblePositions |= Player::First; }
                if (appearances.at2B > 0) { player.eligiblePositions |= Player::Second; }
                if (appearances.atSS > 0) { player.eligiblePositions |= Player::SS; }
                if (appearances.at3B > 0) { player.eligiblePositions |= Player::Third; }
                if (appearances.atOF > 0) { player.eligiblePositions |= Player::Outfield; }
                if (appearances.atDH > 0) { player.eligiblePositions |= Player::DH; }
            } catch (std::runtime_error& e) {
                std::cerr << "[Hitter] " << e.what() << std::endl;
            }

            // Set catergory
            player.catergory = Player::Hitter;

            // Store in vector
            m_vecPlayers.emplace_back(player);

        } catch (...) {

            // Try next if something went wrong
            continue;
        }
    }

    // Calculated zScores
    GET_ZSCORE(m_vecPlayers, hitting.AVG, hitting.zAVG);
    GET_ZSCORE(m_vecPlayers, hitting.HR, hitting.zHR);
    GET_ZSCORE(m_vecPlayers, hitting.R, hitting.zR);
    GET_ZSCORE(m_vecPlayers, hitting.RBI, hitting.zRBI);
    GET_ZSCORE(m_vecPlayers, hitting.SB, hitting.zSB);

    // Calculated weighted zScores
    for (Player& player : m_vecPlayers) {
        player.hitting.wAVG = player.hitting.AB * player.hitting.zAVG;
    }
    GET_ZSCORE(m_vecPlayers, hitting.wAVG, hitting.zAVG);

    // Sum zScores
    for (Player& player : m_vecPlayers) {
        player.zScore = (player.hitting.zAVG + player.hitting.zHR + player.hitting.zR + player.hitting.zRBI + player.hitting.zSB);
    }

    // Re-rank based on z-score
    std::sort(std::begin(m_vecPlayers), std::end(m_vecPlayers), [](const auto& lhs, const auto& rhs) {
        return lhs.zScore > rhs.zScore;
    });

    // Get the "replacement player"
    auto zReplacement = m_vecPlayers[TOTAL_HITTERS].zScore;

    // Scale all players based off the replacement player
    float sumPositiveZScores = 0;
    std::for_each(std::begin(m_vecPlayers), std::end(m_vecPlayers), [&](Player& player) {
        player.zScore -= zReplacement;
        if (player.zScore > 0) {
            sumPositiveZScores += player.zScore;
        }
    });

    // Apply cost ratio
    static const float costPerZ = float(TOTAL_HITTER_MONEY) / sumPositiveZScores;
    std::for_each(std::begin(m_vecPlayers), std::end(m_vecPlayers), [&](Player& player) {
        player.cost = player.zScore * costPerZ;
    });
}

//------------------------------------------------------------------------------
// PlayerTableModel
//------------------------------------------------------------------------------
void PlayerTableModel::LoadPitchingProjections(const std::string& filename, const PlayerApperances& playerApperances)
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

            Player player;
            player.name = QString::fromStdString(parsed[LUT["Name"]]);
            player.team = QString::fromStdString(parsed[LUT["Team"]]);
            player.pitching.IP = boost::lexical_cast<decltype(player.pitching.IP)>(parsed[LUT["IP"]]);
            player.pitching.ERA = boost::lexical_cast<decltype(player.pitching.ERA)>(parsed[LUT["ERA"]]);
            player.pitching.WHIP = boost::lexical_cast<decltype(player.pitching.WHIP)>(parsed[LUT["WHIP"]]);
            player.pitching.W = boost::lexical_cast<decltype(player.pitching.W)>(parsed[LUT["W"]]);
            player.pitching.SO = boost::lexical_cast<decltype(player.pitching.SO)>(parsed[LUT["SO"]]);
            player.pitching.SV = boost::lexical_cast<decltype(player.pitching.SV)>(parsed[LUT["SV"]]);

            if (player.pitching.IP < 5) {
                continue;
            }

            // Lookup appearances 
            const auto& appearances = playerApperances.Lookup(player.name.toStdString());
            if (float(appearances.G) * 0.9f < float(appearances.GS)) {
                player.eligiblePositions |= int32_t(Player::Starter);
            } else {
                player.eligiblePositions|= int32_t(Player::Relief);
            }


            // 
            player.catergory = Player::Pitcher;
                
            m_vecPlayers.emplace_back(player);

        } catch (std::runtime_error& e) {

            std::cerr << "[Pitcher] " << e.what() << std::endl;

        } catch (...) {

            // Try next if something went wrong
            continue;
        }
    }

    // Calculated zScores
    GET_ZSCORE(m_vecPlayers, pitching.SO,   pitching.zSO);
    GET_ZSCORE(m_vecPlayers, pitching.W,    pitching.zW);
    GET_ZSCORE(m_vecPlayers, pitching.SV,   pitching.zSV);
    GET_ZSCORE(m_vecPlayers, pitching.ERA,  pitching.zERA);
    GET_ZSCORE(m_vecPlayers, pitching.WHIP, pitching.zWHIP);

    // Calculated weighted zScores
    for (Player& player : m_vecPlayers) {
        player.pitching.wERA = player.pitching.IP * player.pitching.zERA;
        player.pitching.wWHIP = player.pitching.IP * player.pitching.zWHIP;
    }
    GET_ZSCORE(m_vecPlayers, pitching.wERA,  pitching.zERA);
    GET_ZSCORE(m_vecPlayers, pitching.wWHIP, pitching.zWHIP);

    // Invert ERA and WHIP... lower is better
    for (Player& player : m_vecPlayers) {
        player.pitching.zERA =  -player.pitching.zERA;
        player.pitching.zWHIP = -player.pitching.zWHIP;
    }

    // Sum zscore
    for (Player& player : m_vecPlayers) {
        player.zScore = (player.pitching.zSO + player.pitching.zW + player.pitching.zSV + player.pitching.zERA + player.pitching.zWHIP);
    }

    // Re-rank based on z-score
    std::sort(m_vecPlayers.begin(), m_vecPlayers.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.zScore > rhs.zScore;
    });

    // Get the "replacement player"
    auto zReplacement = m_vecPlayers[TOTAL_PITCHERS].zScore;

    // Scale all players based off the replacement player
    float sumPositiveZScores = 0;
    std::for_each(std::begin(m_vecPlayers), std::end(m_vecPlayers), [&](Player& pitcher) {
        pitcher.zScore -= zReplacement;
        if (pitcher.zScore > 0) {
            sumPositiveZScores += pitcher.zScore;
        }
    });

    // Apply cost ratio
    static const float costPerZ = float(TOTAL_PITCHER_MONEY) / sumPositiveZScores;
    std::for_each(std::begin(m_vecPlayers), std::end(m_vecPlayers), [&](Player& pitcher) {
        pitcher.cost = pitcher.zScore * costPerZ;
    });
}

//------------------------------------------------------------------------------
// rowCount (override)
//------------------------------------------------------------------------------
int PlayerTableModel::rowCount(const QModelIndex& index) const 
{
    return int(m_vecPlayers.size());
}

//------------------------------------------------------------------------------
// columnCount (override)
//------------------------------------------------------------------------------
int PlayerTableModel::columnCount(const QModelIndex& index) const 
{
    return COLUMN_COUNT;
}

//------------------------------------------------------------------------------
// data (override)
//------------------------------------------------------------------------------
QVariant PlayerTableModel::data(const QModelIndex& index, int role) const
{
    const Player& player = m_vecPlayers.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole || role == RawDataRole) {

        switch (index.column())
        {
        case COLUMN_RANK:
            return index.row() + 1;
        case COLUMN_DRAFT_BUTTON:
            return uint32_t(player.status);
        case COLUMN_OWNER:
            if (role == RawDataRole) {
                return player.ownerId;
            } else {
                return GetOwnerName(player.ownerId);
            }
        case COLUMN_PAID:
            if (role == RawDataRole) {
                return player.paid;
            } else {
                return player.paid != 0 ? QString("$%1").arg(player.paid) : QString("--");
            }
        case COLUMN_DRAFT_POSITION:
            if (role == RawDataRole) {
                return player.draftPosition;
            } else {
                return PositionToString(player.draftPosition);
            }
        case COLUMN_NAME:
            return player.name;
        case COLUMN_TEAM:
            return player.team;
        case COLUMN_CATERGORY:
            return player.catergory;
        case COLUMN_POSITION:
            if (role == RawDataRole) {
                return player.eligiblePositions;
            } else {
                return PositionToString(player.eligiblePositions);
            }
        case COLUMN_AB:
            return player.hitting.AB;
        case COLUMN_AVG:
            if (role == RawDataRole) {
                return player.hitting.AVG;
            } else {
                return QString::number(player.hitting.AVG, 'f', 3);
            }
        case COLUMN_HR:
            return player.hitting.HR;
        case COLUMN_R:
            return player.hitting.R;
        case COLUMN_RBI:
            return player.hitting.RBI;
        case COLUMN_SB:
            return player.hitting.SB;
        case COLUMN_ESTIMATE:
            if (role == RawDataRole) {
                return player.cost;
            } else {
                return QString("$%1").arg(QString::number(player.cost, 'f', 2));
            }
        case COLUMN_Z:
            if (role == RawDataRole) {
                return player.zScore;
            } else {
                return QString::number(player.zScore, 'f', 3);
            }
        case COLUMN_COMMENT:
            return player.comment;
        }
    } 
    
    if (role == Qt::TextAlignmentRole) {

        switch (index.column())
        {
        case COLUMN_PAID:
        case COLUMN_OWNER:
        case COLUMN_DRAFT_POSITION:
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
                .arg(player.hitting.zAVG)
                .arg(player.hitting.zR)
                .arg(player.hitting.zRBI)
                .arg(player.hitting.zHR)
                .arg(player.hitting.zSB);
        }
    }

    return QVariant();
}

//------------------------------------------------------------------------------
// headerData (override)
//------------------------------------------------------------------------------
QVariant PlayerTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                return "Pos.";
            case COLUMN_NAME:
                return "Name";
            case COLUMN_TEAM:
                return "Team";
            case COLUMN_CATERGORY:
                return "Catergory";
            case COLUMN_POSITION:
                return "Pos.";
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
Qt::ItemFlags PlayerTableModel::flags(const QModelIndex &index) const
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
bool PlayerTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    Player& player = m_vecPlayers.at(index.row());

    switch (index.column())
    {
    case COLUMN_DRAFT_BUTTON:
        player.status = Player::Status(value.toInt());
        return true;
    case COLUMN_PAID:
        player.paid = value.toInt();
        return true;
    case COLUMN_OWNER:
        player.ownerId = value.toInt();
        return true;
    default:
        return false;
    }

}
