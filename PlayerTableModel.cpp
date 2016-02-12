#include "PlayerTableModel.h"
#include "ZScore.h"
#include "Teams.h"
#include "Player.h"
#include "DraftSettings.h"
#include "OwnerSortFilterProxyModel.h"

#include <vector>
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <QAbstractItemDelegate>
#include <QFile>
#include <QTextStream>
#include <QIcon>

//------------------------------------------------------------------------------
// PositionToString (static helper)
//------------------------------------------------------------------------------
template <typename T>
static T SafeLexicalCast(const std::string& x)
{
    if (x == std::string(" ")) {
        return T(0);
    }

    try {
        return boost::lexical_cast<T>(x);
    } catch (...) {
        return T(0);
    }
}

//------------------------------------------------------------------------------
// PositionToString (static helper)
//------------------------------------------------------------------------------
QString PositionToString(const PlayerPosition& position)
{
    switch (position)
    {
    case PlayerPosition::None: return "--";
    case PlayerPosition::Catcher: return "C";
    case PlayerPosition::First: return "1B";
    case PlayerPosition::Second: return "2B";
    case PlayerPosition::SS: return "SS";
    case PlayerPosition::Third: return "3B";
    case PlayerPosition::MiddleInfield: return "MI";
    case PlayerPosition::CornerInfield: return "CI";
    case PlayerPosition::Outfield: return "OF";
    case PlayerPosition::Utility: return "U";
    case PlayerPosition::DH: return "DH";
    case PlayerPosition::Starter: return "SP";
    case PlayerPosition::Relief: return "RP";
    case PlayerPosition::Pitcher: return "P";
    default:
        return "??";
        break;
    }
}

//------------------------------------------------------------------------------
// PositionMaskToStringList (static helper)
//------------------------------------------------------------------------------
QStringList PositionMaskToStringList(const PlayerPositionBitfield& positionBitField)
{
    QStringList vecPos;
    for (auto i = 0; i < uint32_t(PlayerPosition::COUNT); i++) {
        if (positionBitField & (1 << i)) {
            vecPos.push_back(PositionToString(PlayerPosition(i)));
        }
    }
    return vecPos;
}

//------------------------------------------------------------------------------
// PositionToString (static helper)
//------------------------------------------------------------------------------
PlayerPosition StringToPosition(const QString& position)
{
    if (position == "C")  { return PlayerPosition::Catcher; }
    if (position == "1B") { return PlayerPosition::First; }
    if (position == "2B") { return PlayerPosition::Second; }
    if (position == "SS") { return PlayerPosition::SS; }
    if (position == "3B") { return PlayerPosition::Third; }
    if (position == "OF") { return PlayerPosition::Outfield; }
    if (position == "DH") { return PlayerPosition::DH; }
    if (position == "MI") { return PlayerPosition::MiddleInfield; }
    if (position == "CI") { return PlayerPosition::CornerInfield; }
    if (position == "U")  { return PlayerPosition::Utility; }
    if (position == "SP") { return PlayerPosition::Starter; }
    if (position == "RP") { return PlayerPosition::Relief; }
    if (position == "P")  { return PlayerPosition::Pitcher; }
    return PlayerPosition::None;
}

//------------------------------------------------------------------------------
// PlayerTableModel
//------------------------------------------------------------------------------
PlayerTableModel::PlayerTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    m_vecTargetValues.fill(0);
}

//------------------------------------------------------------------------------
// LoadHittingProjections
//------------------------------------------------------------------------------
void PlayerTableModel::LoadHittingProjections(const std::string& filename, const PlayerApperances& playerApperances)
{
    // Open file
    std::fstream batters(filename);
    std::string row;

    // Vector of new hitters
    std::vector<Player> vecHitters;

    // Tokenize header data
    using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;
    std::getline(batters, row);
    Tokenizer tokenizer(row);

    // Stats to find
    std::unordered_map<std::string, uint32_t> LUT =
    {
        { "playerid", 0 },
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

            // Parse into player data
            Player player;
            player.id =  QString::fromStdString(parsed[LUT["playerid"]]);
            player.name = QString::fromStdString(parsed[LUT["Name"]]);
            player.team = QString::fromStdString(parsed[LUT["Team"]]);
            player.hitting.PA = SafeLexicalCast<uint32_t>(parsed[LUT["PA"]]);
            player.hitting.AB = SafeLexicalCast<uint32_t>(parsed[LUT["AB"]]);
            player.hitting.H = SafeLexicalCast<uint32_t>(parsed[LUT["H"]]);
            player.hitting.HR = SafeLexicalCast<uint32_t>(parsed[LUT["HR"]]);
            player.hitting.R = SafeLexicalCast<uint32_t>(parsed[LUT["R"]]);
            player.hitting.RBI = SafeLexicalCast<uint32_t>(parsed[LUT["RBI"]]);
            player.hitting.SB = SafeLexicalCast<uint32_t>(parsed[LUT["SB"]]);
            player.hitting.AVG = float(player.hitting.H) / float(player.hitting.AB);

            // Skip players with little-to-no AB
            if (player.hitting.AB < 30) {
                continue;
            }

            // Lookup appearances 
            try {
                const auto& appearances = playerApperances.Lookup(player.name.toStdString());
                if (appearances.atC > 0 ) { player.eligiblePositionBitfield |= ToBitfield({PlayerPosition::Catcher, PlayerPosition::Utility}); }
                if (appearances.at1B > 0) { player.eligiblePositionBitfield |= ToBitfield({PlayerPosition::First, PlayerPosition::Utility, PlayerPosition::CornerInfield}); }
                if (appearances.at2B > 0) { player.eligiblePositionBitfield |= ToBitfield({PlayerPosition::Second, PlayerPosition::Utility, PlayerPosition::MiddleInfield}); }
                if (appearances.atSS > 0) { player.eligiblePositionBitfield |= ToBitfield({PlayerPosition::SS, PlayerPosition::Utility, PlayerPosition::MiddleInfield}); }
                if (appearances.at3B > 0) { player.eligiblePositionBitfield |= ToBitfield({PlayerPosition::Third, PlayerPosition::Utility, PlayerPosition::CornerInfield}); }
                if (appearances.atOF > 0) { player.eligiblePositionBitfield |= ToBitfield({PlayerPosition::Outfield, PlayerPosition::Utility}); }

                // XXX: Don't care about DL
                // if (appearances.atDH > 0) { player.eligiblePositionBitfield |= ToBitfield(PlayerPosition::DH); }

            } catch (std::runtime_error& e) {
                std::cerr << "[Hitter] " << e.what() << std::endl;
            }

            // Set catergory
            player.catergory = Player::Catergory::Hitter;

            // XXX: NL-only
            if (LookupTeamGroup(player.team.toStdString()).leauge != Leauge::NL) {
                continue;
            }

            // Store in vector
            vecHitters.emplace_back(player);

        } catch (...) {

            // Try next if something went wrong
            continue;
        }
    }

    // Calculated zScores
    GET_ZSCORE(vecHitters, vecHitters.size(), hitting.AVG, hitting.zAVG);
    GET_ZSCORE(vecHitters, vecHitters.size(), hitting.HR, hitting.zHR);
    GET_ZSCORE(vecHitters, vecHitters.size(), hitting.R, hitting.zR);
    GET_ZSCORE(vecHitters, vecHitters.size(), hitting.RBI, hitting.zRBI);
    GET_ZSCORE(vecHitters, vecHitters.size(), hitting.SB, hitting.zSB);

    // Calculated weighted zScores
    for (Player& player : vecHitters) {
        player.hitting.wAVG = player.hitting.AB * player.hitting.zAVG;
    }
    GET_ZSCORE(vecHitters, vecHitters.size(), hitting.wAVG, hitting.zAVG);

    // Some SGP formulas...
    //
    // RSGP   =[@R] / 24.6
    // HRSGP  =[@HR] / 10.4
    // RBISGP =[@RBI] / 24.6
    // SBSGP  =[@SB] / 9.4
    // AVGSGP =(([@H] + 1768) / ([@AB] + 6617) - 0.267) / 0.0024
    
    // for (Player& player : vecHitters) {
    //     player.hitting.zR   = player.hitting.R / 30.983f;
    //     player.hitting.zHR  = player.hitting.HR / 9.1958f;
    //     player.hitting.zRBI = player.hitting.RBI / 30.105f;
    //     player.hitting.zSB  = player.hitting.SB / 7.818f;
    //     player.hitting.zAVG = ((player.hitting.H + 1446.095f) / (player.hitting.AB + 5536.386f) - 0.261184f) / 0.002863636f;
    // }

    // Sum zScores
    for (Player& player : vecHitters) {
        player.zScore = (player.hitting.zAVG + player.hitting.zHR + player.hitting.zR + player.hitting.zRBI + player.hitting.zSB);
    }

    // Re-rank based on z-score
    std::sort(std::begin(vecHitters), std::end(vecHitters), [](const auto& lhs, const auto& rhs) {
        return lhs.zScore > rhs.zScore;
    });

    // Set catergory rank
    for (uint32_t i = 0; i < vecHitters.size(); i++) {
        vecHitters[i].categoryRank = i + 1;
    }
    
    // Get the "replacement player"
    auto totalHitters = DraftSettings::HitterCount() * DraftSettings::OwnerCount();
    auto zReplacement = vecHitters[totalHitters].zScore;

    // Scale all players based off the replacement player
    float sumPositiveZScores = 0;
    std::for_each(std::begin(vecHitters), std::end(vecHitters), [&](Player& hitter) {
        auto zDiff = hitter.zScore - zReplacement;
        if (zDiff > 0) {
            sumPositiveZScores += zDiff;
            m_vecTargetValues[COLUMN_R]   += hitter.hitting.R;
            m_vecTargetValues[COLUMN_HR]  += hitter.hitting.HR;
            m_vecTargetValues[COLUMN_RBI] += hitter.hitting.RBI;
            m_vecTargetValues[COLUMN_SB]  += hitter.hitting.SB;
            m_vecTargetValues[COLUMN_H]   += hitter.hitting.H;
            m_vecTargetValues[COLUMN_AB]  += hitter.hitting.AB;
        }
    });

    // Apply cost ratio
    float totalHitterMoney =  DraftSettings::HittingSplit() * DraftSettings::Budget() * DraftSettings::OwnerCount();
    float costPerZ = float(totalHitterMoney) / sumPositiveZScores;
    std::for_each(std::begin(vecHitters), std::end(vecHitters), [&](Player& player) {
        player.cost = (player.zScore - zReplacement) * costPerZ;
    });

    // Add to main storage
    for (const Player& player : vecHitters) {
        m_vecPlayers.push_back(player);
    }
}

//------------------------------------------------------------------------------
// LoadPitchingProjections
//------------------------------------------------------------------------------
void PlayerTableModel::LoadPitchingProjections(const std::string& filename, const PlayerApperances& playerApperances)
{
    // Open file
    std::fstream pitchers(filename);
    std::string row;
    
    // Vector of new hitters
    std::vector<Player> vecPitchers;

    // Tokenize header data
    using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;
    std::getline(pitchers, row);
    Tokenizer tokenizer(row);

    std::unordered_map<std::string, uint32_t> LUT =
    {
        { "playerid", 0 },
        { "Name",0 },
        { "Team",0 },
        { "IP",0 },
        { "H",0 },
        { "BB",0 },
        { "SO",0 },
        { "ER", 0},
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
            player.id = QString::fromStdString(parsed[LUT["playerid"]]);
            player.name = QString::fromStdString(parsed[LUT["Name"]]);
            player.team = QString::fromStdString(parsed[LUT["Team"]]);
            player.pitching.IP = SafeLexicalCast<decltype(player.pitching.IP)>(parsed[LUT["IP"]]);
            player.pitching.ER = SafeLexicalCast<decltype(player.pitching.ER)>(parsed[LUT["ER"]]);
            player.pitching.H = SafeLexicalCast<decltype(player.pitching.H)>(parsed[LUT["H"]]);
            player.pitching.BB = SafeLexicalCast<decltype(player.pitching.BB)>(parsed[LUT["BB"]]);
            player.pitching.ERA = SafeLexicalCast<decltype(player.pitching.ERA)>(parsed[LUT["ERA"]]);
            player.pitching.WHIP = SafeLexicalCast<decltype(player.pitching.WHIP)>(parsed[LUT["WHIP"]]);
            player.pitching.W = SafeLexicalCast<decltype(player.pitching.W)>(parsed[LUT["W"]]);
            player.pitching.SO = SafeLexicalCast<decltype(player.pitching.SO)>(parsed[LUT["SO"]]);
            player.pitching.SV = SafeLexicalCast<decltype(player.pitching.SV)>(parsed[LUT["SV"]]);

            // Ignore pitchers with no innings
            if (player.pitching.IP < 5) {
                continue;
            }

            // Always a pitcher
            player.eligiblePositionBitfield |= ToBitfield(PlayerPosition::Pitcher);

            // XXX: Don't care about SP/RP
            // Lookup appearances 
            // const auto& appearances = playerApperances.Lookup(player.name.toStdString());
            // if (float(appearances.G) * 0.7f < float(appearances.GS)) {
            //     player.eligiblePositionBitfield |= ToBitfield(PlayerPosition::Starter);
            // } else {
            //     player.eligiblePositionBitfield |= ToBitfield(PlayerPosition::Relief);
            // }

            // XXX: NL-only!
            if (LookupTeamGroup(player.team.toStdString()).leauge != Leauge::NL) {
                continue;
            }

            // Set catergory
            player.catergory = Player::Catergory::Pitcher;
            
            // Add to pitchers
            vecPitchers.emplace_back(player);

        } catch (std::runtime_error& e) {

            std::cerr << "[Pitcher] " << e.what() << std::endl;

        } catch (...) {

            // Try next if something went wrong
            continue;
        }
    }

    // Calculated zScores
    GET_ZSCORE(vecPitchers, vecPitchers.size(), pitching.SO, pitching.zSO);
    GET_ZSCORE(vecPitchers, vecPitchers.size(), pitching.W, pitching.zW);
    GET_ZSCORE(vecPitchers, vecPitchers.size(), pitching.SV, pitching.zSV);
    GET_ZSCORE(vecPitchers, vecPitchers.size(), pitching.ERA, pitching.zERA);
    GET_ZSCORE(vecPitchers, vecPitchers.size(), pitching.WHIP, pitching.zWHIP);

    // Calculated weighted zScores
    for (Player& player : vecPitchers) {
        player.pitching.wERA = player.pitching.IP * player.pitching.zERA;
        player.pitching.wWHIP = player.pitching.IP * player.pitching.zWHIP;
    }
    GET_ZSCORE(vecPitchers, vecPitchers.size(), pitching.wERA, pitching.zERA);
    GET_ZSCORE(vecPitchers, vecPitchers.size(), pitching.wWHIP, pitching.zWHIP);

    // Invert ERA and WHIP... lower is better
    for (Player& player : vecPitchers) {
        player.pitching.zERA =  -player.pitching.zERA;
        player.pitching.zWHIP = -player.pitching.zWHIP;
    }

    // SPG Formulas 
    //
    // WSGP    =[@W] / 3.03
    // SVSGP   =[@SV] / 9.95
    // SOSGP   =[@SO] / 39.3
    // ERASGP  =((475 + [@ER]) * 9 / (1192 + [@IP]) - 3.59) / -0.076
    // WHIPSGP =((1466 + [@H] + [@BB]) / (1192 + [@IP]) - 1.23) / -0.015

    // for (Player& player : vecPitchers) {
    //     player.pitching.zW    = player.pitching.W / 3.03;
    //     player.pitching.zSV   = player.pitching.SV / 9.95;
    //     player.pitching.zSO   = player.pitching.SO / 39.3;
    //     player.pitching.zERA  = ((475 + player.pitching.ER) * 9 / (1192 + player.pitching.IP) - 3.59) / -0.076;
    //     player.pitching.zWHIP = ((1466 + player.pitching.H + player.pitching.BB) / (1192 + player.pitching.IP) - 1.23) / -0.015;
    // }

    // Sum z-score
    for (Player& player : vecPitchers) {
        player.zScore = (player.pitching.zSO + player.pitching.zW + player.pitching.zSV + player.pitching.zERA + player.pitching.zWHIP);
    }

    // Re-rank based on z-score
    std::sort(vecPitchers.begin(), vecPitchers.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.zScore > rhs.zScore;
    });

    // Set catergory rank
    for (uint32_t i = 0; i < vecPitchers.size(); i++) {
        vecPitchers[i].categoryRank = i + 1;
    }

    // Get the "replacement player"
    auto totalPitcher = DraftSettings::PitcherCount() * DraftSettings::OwnerCount();
    auto zReplacement = vecPitchers[totalPitcher].zScore;

    // Scale all players based off the replacement player
    float sumPositiveZScores = 0;
    std::for_each(std::begin(vecPitchers), std::end(vecPitchers), [&](Player& pitcher) {
        auto zDiff = pitcher.zScore - zReplacement;
        if (zDiff > 0) {
            sumPositiveZScores += zDiff;
            m_vecTargetValues[COLUMN_W]  += pitcher.pitching.W;
            m_vecTargetValues[COLUMN_SV] += pitcher.pitching.SV;
            m_vecTargetValues[COLUMN_SO] += pitcher.pitching.SO;
            m_vecTargetValues[COLUMN_HA] += pitcher.pitching.H;
            m_vecTargetValues[COLUMN_BB] += pitcher.pitching.BB;
            m_vecTargetValues[COLUMN_IP] += pitcher.pitching.IP;
            m_vecTargetValues[COLUMN_ER] += pitcher.pitching.ER;
        }
    });

    // Apply cost ratio
    float totalPitcherMoney = DraftSettings::PitchingSplit() * DraftSettings::Budget() * DraftSettings::OwnerCount();
    float costPerZ = float(totalPitcherMoney) / sumPositiveZScores;
    std::for_each(std::begin(vecPitchers), std::end(vecPitchers), [&](Player& pitcher) {
        pitcher.cost = (pitcher.zScore - zReplacement) * costPerZ;
    });

    // Add to main storage
    for (const Player& player : vecPitchers) {
        m_vecPlayers.push_back(player);
    }
}

//------------------------------------------------------------------------------
// SaveStatus
//------------------------------------------------------------------------------
bool PlayerTableModel::SaveDraftStatus(const QString& filename) const
{
    // Open file
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    // Output steam
    QTextStream stream(&file);

    // Header information
    stream << "\"playerid\"" << "," ;
    stream << "\"ownerid\"" << ",";
    stream << "\"draftposition\"" << ",";
    stream << "\"paid\"" << endl;

    // Per-drafted player
    for (const auto& player : m_vecPlayers) {
        if (player.ownerId) {
            stream << player.id << ",";
            stream << player.ownerId << ",";
            stream << uint32_t(player.draftPosition) << ",";
            stream << player.paid << endl;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
// ReadDraftStatus
//------------------------------------------------------------------------------
bool PlayerTableModel::LoadDraftStatus(const QString& filename)
{
    // Open file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // Parse header
    QByteArray line = file.readLine();
    QList<QByteArray> wordList = line.split(',');
    
    // TODO: assert header format

    // Parse each line
    while (!file.atEnd()) {

        // Get data
        QByteArray line = file.readLine();
        QList<QByteArray> wordList = line.split(',');

        // parse data
        auto playerId = QString::fromUtf8(wordList.at(0));
        auto ownerId = QString::fromUtf8(wordList.at(1)).toUInt();
        auto draftPosition = QString::fromUtf8(wordList.at(2)).toUInt();
        auto paid = QString::fromUtf8(wordList.at(3)).toUInt();

        // Find this player
        auto itr = std::find_if(m_vecPlayers.begin(), m_vecPlayers.end(), [=](const Player& player) {
            return player.id == playerId;
        });

        // Make sure we find via id
        if (itr == m_vecPlayers.end()) {
            continue;
        }

        // Get row
        auto row = std::distance(m_vecPlayers.begin(), itr);

        // Format results
        DraftDialog::Results results;
        results.ownerId = ownerId;
        results.cost = paid;
        results.position = PlayerPosition(draftPosition);

        // Broadcast
        OnDrafted(results, index(row, 0), this);
    }

    return true;
}

//------------------------------------------------------------------------------
// InitializeTargetValues
//------------------------------------------------------------------------------
void PlayerTableModel::InitializeTargetValues()
{
    // Calculate compound stats
    m_vecTargetValues[COLUMN_AVG] = m_vecTargetValues[COLUMN_H] / m_vecTargetValues[COLUMN_AB];
    m_vecTargetValues[COLUMN_HR] /= float(DraftSettings::OwnerCount());
    m_vecTargetValues[COLUMN_RBI] /= float(DraftSettings::OwnerCount());
    m_vecTargetValues[COLUMN_SB] /= float(DraftSettings::OwnerCount());
    m_vecTargetValues[COLUMN_R] /= float(DraftSettings::OwnerCount());
    m_vecTargetValues[COLUMN_SO] /= float(DraftSettings::OwnerCount());
    m_vecTargetValues[COLUMN_W] /= float(DraftSettings::OwnerCount());
    m_vecTargetValues[COLUMN_SV] /= float(DraftSettings::OwnerCount());
    m_vecTargetValues[COLUMN_WHIP] = (m_vecTargetValues[COLUMN_HA] + m_vecTargetValues[COLUMN_BB]) / m_vecTargetValues[COLUMN_IP];
    m_vecTargetValues[COLUMN_ERA] = (9.f * m_vecTargetValues[COLUMN_ER]) / m_vecTargetValues[COLUMN_IP];
}

//------------------------------------------------------------------------------
// GetTargetValue
//------------------------------------------------------------------------------
float PlayerTableModel::GetTargetValue(enum COLUMN stat) const
{
    return m_vecTargetValues[stat];
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

    if (role == Qt::FontRole && index.column() == COLUMN_ID_LINK) {
        QFont font;
        font.setUnderline(true);
        return QVariant::fromValue(font);
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole || role == RawDataRole) {

        switch (index.column())
        {
        case COLUMN_FLAG:
            if (role == RawDataRole) {
                return player.flag;
            }
            break;
        case COLUMN_RANK:
            return player.categoryRank;
        case COLUMN_OWNER:
            if (role == RawDataRole) {
                return player.ownerId;
            } else {
                return DraftSettings::OwnerAbbreviation(player.ownerId);
            }
        case COLUMN_PAID:
            if (role == RawDataRole) {
                return player.paid;
            } else {
                return player.paid != 0 ? QString("$%1").arg(player.paid) : QString("--");
            }
        case COLUMN_DRAFT_POSITION:
            if (role == RawDataRole) {
                return uint32_t(player.draftPosition);
            } else {
                return PositionToString(player.draftPosition);
            }
        case COLUMN_ID_LINK:
            return player.id;
        case COLUMN_NAME:
            return player.name;
        case COLUMN_TEAM:
            return player.team;
        case COLUMN_CATERGORY:
            return player.catergory;
        case COLUMN_POSITION:
            if (role == RawDataRole) {
                return player.eligiblePositionBitfield;
            } else {
                auto positionsToHide = { PlayerPosition::MiddleInfield, PlayerPosition::CornerInfield, PlayerPosition::Utility };
                auto trimPositions = player.eligiblePositionBitfield & ~ToBitfield(positionsToHide);
                return PositionMaskToStringList(trimPositions).join(", ");
            }
        case COLUMN_AB:
            return player.hitting.AB;
        case COLUMN_H:
            return player.hitting.H;
        case COLUMN_AVG:
            if (role == RawDataRole) {
                return player.hitting.zAVG;
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

        case COLUMN_IP:
            if (role == RawDataRole) {
                return player.pitching.IP;
            } else {
                QString::number(player.pitching.IP, 'f', 1);
            }
        case COLUMN_HA:
            return player.pitching.H;
        case COLUMN_BB:
            return player.pitching.BB;
        case COLUMN_ER:
            return player.pitching.ER;
        case COLUMN_SO:
            return player.pitching.SO;
        case COLUMN_ERA:
            if (role == RawDataRole) {
                return player.pitching.zERA;
            } else {
                return QString::number(player.pitching.ERA, 'f', 3);
            }
        case COLUMN_WHIP:
            if (role == RawDataRole) {
                return player.pitching.zWHIP;
            } else {
                return QString::number(player.pitching.WHIP, 'f', 3);
            }
        case COLUMN_W:
            return player.pitching.W;
        case COLUMN_SV:
            return player.pitching.SV;
        case COLUMN_ESTIMATE:
            if (role == RawDataRole) {
                return player.cost;
            } else {
                float toShow = player.ownerId != 0 ? player.cost : (m_inflationFactor * player.cost);
                return QString("$%1").arg(QString::number(toShow, 'f', 2));
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

    // Gray out taken players
    if (role == Qt::TextColorRole) {
        if (player.ownerId != 0) {
            return QColor(Qt::darkGray);
        }
    }
    
    if (role == Qt::TextAlignmentRole) {

        switch (index.column())
        {
        case COLUMN_PAID:
        case COLUMN_OWNER:
        case COLUMN_DRAFT_POSITION:
        case COLUMN_ID_LINK:
        case COLUMN_FLAG:
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

        switch (index.column())
        {
            case COLUMN_Z:
            {
                if (player.catergory == Player::Catergory::Hitter) {
                    return QString(
                        "zAVG: %1\n"
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

                if (player.catergory == Player::Catergory::Pitcher) {
                    return QString(
                        "zERA:  %1\n"
                        "zSO:   %2\n"
                        "zWHIP: %3\n"
                        "zW:    %4\n"
                        "zSV:   %5")
                        .arg(player.pitching.zERA)
                        .arg(player.pitching.zSO)
                        .arg(player.pitching.zWHIP)
                        .arg(player.pitching.zW)
                        .arg(player.pitching.zSV);
                }
            }

            case COLUMN_ESTIMATE:
            {
                const float baseCost = player.cost;
                const float inflatedCost = m_inflationFactor * baseCost;
                const float diff = inflatedCost - baseCost;
                return QString(
                    "Inflated: $%1\n"
                    "Base:     $%2\n"
                    "Diff:     $%3")
                    .arg(QString::number(inflatedCost, 'f', 2))
                    .arg(QString::number(baseCost, 'f', 2))
                    .arg(QString::number(diff, 'f', 2));
            }
        }
    }

    if (role == Qt::DecorationRole) {

        switch (index.column())
        {
        case COLUMN_FLAG:
        {
            switch (player.flag)
            {
            case Player::FLAG_NONE:
                return QIcon(":/icons/none_10x10.png");
            case Player::FLAG_STAR:
                return QIcon(":/icons/star_10x10.png");
            case Player::FLAG_WATCH:
                return QIcon(":/icons/watch_10x10.png");
            case Player::FLAG_AVOID:
                return QIcon(":/icons/avoid_10x10.png");
            default:
                break;
            }
        }
        default:
            break;
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

        if (role == Qt::InitialSortOrderRole) {
            return section;
        }

        if (role == Qt::DisplayRole) {

            switch (section) 
            {
            case COLUMN_FLAG:
                return "-";
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
            case COLUMN_ID_LINK:
                return "Id.";
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
            case COLUMN_H:
                return "H";
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
            case COLUMN_IP:
                return "IP";
            case COLUMN_HA:
                return "HA";
            case COLUMN_BB:
                return "BB";
            case COLUMN_ER:
                return "ER";
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
            case COLUMN_ESTIMATE:
                return "$";
            case COLUMN_Z:
                return "zScore";
            case COLUMN_COMMENT:
                return "Comment";
            }
        }

        if (role == ChartFormatRole) {

            switch (section)
            {
            case COLUMN_PAID:
            case COLUMN_ESTIMATE:
                return "$%d";

            case COLUMN_AVG:
            case COLUMN_ERA:
            case COLUMN_WHIP:
            case COLUMN_Z:
                return "%0.3f";

            case COLUMN_RANK:
            case COLUMN_AB:
            case COLUMN_HR:
            case COLUMN_R:
            case COLUMN_RBI:
            case COLUMN_SB:
            case COLUMN_IP:
            case COLUMN_SO:
            case COLUMN_W:
            case COLUMN_SV:
            case COLUMN_H:
            case COLUMN_HA:
            case COLUMN_BB:
                return "%i";

            default:
                return "??";
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
    // Get player
    Player& player = m_vecPlayers[index.row()];

    // Only implemented for the flag column
    switch (index.column())
    {
    case COLUMN_FLAG:
        player.flag = value.toUInt();
        break;
    default:
        break;
    }

    emit dataChanged(index, index);
    return QAbstractItemModel::setData(index, value, role);
}

//------------------------------------------------------------------------------
// OnDrafted (override)
//------------------------------------------------------------------------------
void PlayerTableModel::OnDrafted(const DraftDialog::Results& results, const QModelIndex& index, QAbstractItemModel* model)
{
    // Get player
    Player& player = m_vecPlayers[index.row()];

    // Update status
    player.ownerId = results.ownerId;
    player.paid = results.cost;
    player.draftPosition = results.position;

    // Update inflation
    float sumCost = DraftSettings::OwnerCount() * DraftSettings::Budget();
    float sumValue = DraftSettings::OwnerCount() * DraftSettings::Budget();
    for (auto& player : m_vecPlayers)
    {
        if (player.ownerId != 0) {
            sumCost -= player.paid;
            sumValue -= player.cost;
        }
    }
    m_inflationFactor = sumValue ? (sumCost / sumValue) : 1.0f;
    m_inflationFactor = std::max(m_inflationFactor, 0.5);
    m_inflationFactor = std::min(m_inflationFactor, 2.0);
    
    // All the data is changing
    // XXX: this is overkill...
    emit beginResetModel();

    // Broadcast results
    emit DraftedBegin();
    emit Drafted(results, index, model);
    emit DraftedEnd();

    // Update table view
    emit endResetModel();
}

//------------------------------------------------------------------------------
// DraftRandom (Fun!)
//------------------------------------------------------------------------------
void PlayerTableModel::DraftRandom()
{
    for (auto i = 0; i < 512; i++) {
        
        uint32_t r = std::floor((rand() / float(RAND_MAX)) * m_vecPlayers.size());
        auto& player = m_vecPlayers[r];

        auto j = ((r * 113) % DraftSettings::OwnerCount()) + 1;
        
        if (player.cost < 0) {
            continue;
        }

        OwnerSortFilterProxyModel* ownerSortFilterProxyModel = new OwnerSortFilterProxyModel(j, this, this);
        auto count = ownerSortFilterProxyModel->Count(player.catergory);
        auto budget = ownerSortFilterProxyModel->GetRemainingBudget();
        switch (player.catergory)
        {
        case Player::Pitcher:
            if (count > DraftSettings::PitcherCount()) {
                continue;
            }
        case Player::Hitter:
            if (count > DraftSettings::HitterCount()) {
                continue;
            }
        default:
            break;
        }

        auto paid = std::max(std::floor(player.cost), 1.f);
        if (paid > budget) {
            continue;
        }

        DraftDialog::Results results;
        results.cost = paid;
        results.ownerId = j;
        results.position = PlayerPosition::None;
        unsigned long bit = 0;
        _BitScanForward(&bit, player.eligiblePositionBitfield);
        results.position = PlayerPosition(bit);
        OnDrafted(results, index(r, 0), this);
    }

}

#include "PlayerTableModel.moc"