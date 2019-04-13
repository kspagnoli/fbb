#include "FBB/FBBDraftBoardModel.h"
#include "FBB/FBBProjectionService.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"
#include "FBB/FBBApplication.h"

#include <QColor>
#include <QJsonArray>

template <typename T>
static QVariant ToQVariant(T x, int role)
{
    return x;
}

static QVariant ToQVariant(float x, int role)
{
    if (role == Qt::DisplayRole) {
        QString str;
        str.setNum(x, 'f', 3);
        return str;
    }

    return x;
}

FBBDraftBoardModel::FBBDraftBoardModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    m_font = QFont("Consolas", 9);
    m_draftedFont = m_font;
    m_draftedFont.setItalic(true);
}

void FBBDraftBoardModel::Reset(const std::vector<FBBPlayer*>& vecPlayers)
{
    emit beginResetModel();
    m_vecPlayers = vecPlayers;
    CalculateHittingZScores();
    CalculatePitchingZScores();
    emit endResetModel();
}

uint32_t FBBDraftBoardModel::PlayerCount() const
{
    return static_cast<uint32_t>(m_vecPlayers.size());
}

void FBBDraftBoardModel::AddPlayer(FBBPlayer* pPlayer)
{
    emit beginInsertRows(QModelIndex(), PlayerCount(), PlayerCount() + 1);
    return m_vecPlayers.push_back(pPlayer);
    emit endInsertRows();
}

FBBPlayer* FBBDraftBoardModel::GetPlayer(uint32_t index)
{
    if (index >= PlayerCount()) {
        return nullptr;
    }

    return m_vecPlayers[index];
}

std::vector<FBBPlayer*> FBBDraftBoardModel::GetValidHitters()
{
    std::vector<FBBPlayer*> ret;
    for (FBBPlayer* pPlayer : m_vecPlayers) {
        if (pPlayer->type == FBBPlayer::PLAYER_TYPE_HITTER) {
            if (pPlayer->IsValidUnderCurrentSettings()) {
                ret.push_back(pPlayer);
            }
        }
    }
    return ret;
}

std::vector<FBBPlayer*> FBBDraftBoardModel::GetValidPitchers()
{
    std::vector<FBBPlayer*> ret;
    for (FBBPlayer* pPlayer : m_vecPlayers) {
        if (pPlayer->type == FBBPlayer::PLAYER_TYPE_PITCHER) {
            if (pPlayer->IsValidUnderCurrentSettings()) {
                ret.push_back(pPlayer);
            }
        }
    }
    return ret;
}

QJsonObject FBBDraftBoardModel::ToJson() const
{
    QJsonObject jsonArray;
    for (const FBBPlayer* pPlayer : m_vecPlayers)
    {
        if (pPlayer->draftInfo.owner != -1)
        {
            QVariantMap vm;
            vm["owner"] = pPlayer->draftInfo.owner;
            vm["paid"] = pPlayer->draftInfo.paid;
            vm["position"] = pPlayer->draftInfo.position;
            jsonArray[pPlayer->id] = (QJsonObject::fromVariantMap(vm));
        }
    }

    QJsonObject json;
    json["drafted"] = jsonArray;
    return json;
}

int FBBDraftBoardModel::rowCount(const QModelIndex& parent) const
{
    return PlayerCount();
}

int FBBDraftBoardModel::columnCount(const QModelIndex& parent) const
{
    return COLUMN_COUNT;
}

Qt::ItemFlags FBBDraftBoardModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    return flags;
}

QVariant FBBDraftBoardModel::data(const QModelIndex& index, int role) const
{
    // Get player
    const FBBPlayer* pPlayer = m_vecPlayers[index.row()];

    // Sanity check
    if (!pPlayer) {
        return QVariant();
    }

    const bool isHitter = pPlayer->type == FBBPlayer::PLAYER_TYPE_HITTER;
    const bool isPitcher = pPlayer->type == FBBPlayer::PLAYER_TYPE_PITCHER;

    if (role == Qt::DisplayRole || role == RawDataRole) {

        switch (index.column())
        {
        case COLUMN_ID:
            return pPlayer->id;
        case COLUMN_RANK:
            return pPlayer->calculations.rank;
        case COLUMN_OWNER: {
            if (!pPlayer->draftInfo.owner) {
                return "--";
            }
            return fbbApp->Settings()->owners[pPlayer->draftInfo.owner]->abbreviation;
        } break;
        case COLUMN_PAID:
            return pPlayer->draftInfo.paid ? QString("$%1").arg(pPlayer->draftInfo.paid) : "--";
        case COLUMN_NAME:
            return pPlayer->name;
        case COLUMN_TEAM:
            return FBBTeamToString(pPlayer->team);
        case COLUMN_AGE:
            return pPlayer->age;
        case COLUMN_EXPERIENCE:
            return pPlayer->experience;
        case COLUMN_POSITION:
        {
            return FBBPositionMaskToString(pPlayer->EligablePositions(), true);
        }
        case COLUMN_AB:
            if (isHitter) {
                return ToQVariant(pPlayer->projection.hitting.AB, role);
            }
        case COLUMN_H:
            if (isHitter) {
                return ToQVariant(pPlayer->projection.hitting.H, role);
            }
        case COLUMN_AVG:
            if (isHitter) {
                if (m_mode == Mode::STAT) {
                    return ToQVariant(pPlayer->projection.hitting.AVG(), role);
                } else if (m_mode == Mode::Z_SCORE) {
                    return ToQVariant(pPlayer->calculations.zHitting.AVG, role);
                }
            }
            break;
        case COLUMN_HR:
            if (isHitter) {
                if (m_mode == Mode::STAT) {
                    return ToQVariant(pPlayer->projection.hitting.HR, role);
                } else if (m_mode == Mode::Z_SCORE) {
                    return ToQVariant(pPlayer->calculations.zHitting.HR, role);
                }
            }
            break;
        case COLUMN_R:
            if (isHitter) {
                if (m_mode == Mode::STAT) {
                    return ToQVariant(pPlayer->projection.hitting.R, role);
                }else if (m_mode == Mode::Z_SCORE) {
                    return ToQVariant(pPlayer->calculations.zHitting.R, role);
                }
            }
            break;
        case COLUMN_RBI:
            if (isHitter) {
                if (m_mode == Mode::STAT) {
                    return ToQVariant(pPlayer->projection.hitting.RBI, role);
                }else if (m_mode == Mode::Z_SCORE) {
                    return ToQVariant(pPlayer->calculations.zHitting.RBI, role);
                }
            }
            break;
        case COLUMN_SB:
            if (isHitter) {
                if (m_mode == Mode::STAT) {
                    return ToQVariant(pPlayer->projection.hitting.SB, role);
                } else if (m_mode == Mode::Z_SCORE) {
                    return ToQVariant(pPlayer->calculations.zHitting.SB, role);
                }
            }
            break;
        case COLUMN_IP:
            if (isPitcher) {
                return ToQVariant(pPlayer->projection.pitching.IP, role);
            }
        case COLUMN_HA:
            if (isPitcher) {
                return ToQVariant(pPlayer->projection.pitching.H, role);
            }
        case COLUMN_BB:
            if (isPitcher) {
                return ToQVariant(pPlayer->projection.pitching.BB, role);
            }
        case COLUMN_ER:
            if (isPitcher) {
                return ToQVariant(pPlayer->projection.pitching.ER, role);
            }
        case COLUMN_SO:
            if (isPitcher) {
                if (m_mode == Mode::STAT) {
                    return ToQVariant(pPlayer->projection.pitching.SO, role);
                } else if (m_mode == Mode::Z_SCORE) {
                    return ToQVariant(pPlayer->calculations.zPitching.SO, role);
                }
            }
            break;
        case COLUMN_ERA:
            if (isPitcher) {
                if (m_mode == Mode::STAT) {
                    return ToQVariant(pPlayer->projection.pitching.ERA(), role);
                } else if (m_mode == Mode::Z_SCORE) {
                    return ToQVariant(pPlayer->calculations.zPitching.ERA, role);
                }
            }
            break;
        case COLUMN_WHIP:
            if (isPitcher) {
                if (m_mode == Mode::STAT) {
                    return ToQVariant(pPlayer->projection.pitching.WHIP(), role);
                } else if (m_mode == Mode::Z_SCORE) {
                    return ToQVariant(pPlayer->calculations.zPitching.WHIP, role);
                }
            }
            break;
        case COLUMN_W:
            if (isPitcher) {
                if (m_mode == Mode::STAT) {
                    return ToQVariant(pPlayer->projection.pitching.W, role);
                } else if (m_mode == Mode::Z_SCORE) {
                    return ToQVariant(pPlayer->calculations.zPitching.W, role);
                }
            }
            break;
        case COLUMN_SV:
            if (isPitcher) {
                if (m_mode == Mode::STAT) {
                    return ToQVariant(pPlayer->projection.pitching.SV, role);
                } else if (m_mode == Mode::Z_SCORE) {
                    return ToQVariant(pPlayer->calculations.zPitching.SV, role);
                }
            }
            break;
        case COLUMN_ESTIMATE:
            return ToQVariant(pPlayer->calculations.estimate, role);
        case COLUMN_Z: {
            return ToQVariant(pPlayer->calculations.zScore, role);
        } break;
        case COLUMN_COMMENT:
            return "Comment";
        }
    }

    if (role == Qt::TextAlignmentRole) {

        switch (index.column()) {
        
        case COLUMN_NAME:
        case COLUMN_TEAM:
        case COLUMN_POSITION:
        case COLUMN_OWNER:
        case COLUMN_COMMENT:
            return int(Qt::AlignLeft| Qt::AlignVCenter);

        case COLUMN_ID:
        case COLUMN_RANK:
        case COLUMN_PAID:
        case COLUMN_AGE:
        case COLUMN_EXPERIENCE:
        case COLUMN_AB:
        case COLUMN_H:
        case COLUMN_AVG:
        case COLUMN_HR:
        case COLUMN_R:
        case COLUMN_RBI:
        case COLUMN_SB:
        case COLUMN_IP:
        case COLUMN_HA:
        case COLUMN_BB:
        case COLUMN_ER:
        case COLUMN_SO:
        case COLUMN_ERA:
        case COLUMN_WHIP:
        case COLUMN_W:
        case COLUMN_SV:
        case COLUMN_Z:
        case COLUMN_ESTIMATE:
            return int(Qt::AlignRight | Qt::AlignVCenter);
        case COLUMN_COUNT:
            break;
        }
    }

    if (role == Qt::FontRole) {
        if (pPlayer->draftInfo.owner != 0) {
            return m_draftedFont;
        } else {
            return m_font;
        }
    }

    if (role == Qt::ForegroundRole) {
        if (pPlayer->draftInfo.owner != 0) {
            return QColor(Qt::gray);
        }
    }

    return QVariant();
}

QVariant FBBDraftBoardModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::FontRole) {
        return m_font;
    }

    if (orientation == Qt::Horizontal) {

        if (role == Qt::DisplayRole || role == Qt::ToolTipRole) {

            switch (section)
            {
            case COLUMN_RANK:
                return "#";
            case COLUMN_ID:
                return "ID";
            case COLUMN_OWNER:
                return "Owner";
            case COLUMN_PAID:
                return "Paid";
            case COLUMN_NAME:
                return "Name";
            case COLUMN_TEAM:
                return "Team";
            case COLUMN_AGE:
                return "Age";
            case COLUMN_EXPERIENCE:
                return "Exp.";
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
    }

    return QVariant();
}

void FBBDraftBoardModel::SetMode(Mode mode)
{
    m_mode = mode;
    QVector<int> roles = { Qt::DisplayRole };
    emit dataChanged(index(0, COLUMN_FIRST_HITTING), index(PlayerCount() - 1, COLUMN_LAST_PITCHING), roles);
}

void FBBDraftBoardModel::CalculateZScores()
{
    CalculateHittingZScores();
    CalculatePitchingZScores();
}

void FBBDraftBoardModel::CalculateHittingZScores()
{
    std::vector<FBBPlayer*> vecHitters = GetValidHitters();

    struct PerHitting
    {
        double PA = 0;
        double AB = 0;
        double H = 0;
        double _2B = 0;
        double _3B = 0;
        double HR = 0;
        double R = 0;
        double RBI = 0;
        double BB = 0;
        double SO = 0;
        double HBP = 0;
        double SB = 0;
        double CS = 0;
        double AVG = 0;
    };

    PerHitting sumHitting;
    for (FBBPlayer* pHitter : vecHitters) {
        sumHitting.PA += pHitter->projection.hitting.PA;
        sumHitting.AB += pHitter->projection.hitting.AB;
        sumHitting.H += pHitter->projection.hitting.H;
        sumHitting._2B += pHitter->projection.hitting._2B;
        sumHitting._3B += pHitter->projection.hitting._3B;
        sumHitting.HR += pHitter->projection.hitting.HR;
        sumHitting.R += pHitter->projection.hitting.R;
        sumHitting.RBI += pHitter->projection.hitting.RBI;
        sumHitting.BB += pHitter->projection.hitting.BB;
        sumHitting.SO += pHitter->projection.hitting.SO;
        sumHitting.HBP += pHitter->projection.hitting.HBP;
        sumHitting.SB += pHitter->projection.hitting.SB;
        sumHitting.CS += pHitter->projection.hitting.CS;
    }

    PerHitting avgHitting;
    avgHitting.PA = sumHitting.PA / vecHitters.size();
    avgHitting.AB = sumHitting.AB / vecHitters.size();
    avgHitting.H = sumHitting.H / vecHitters.size();
    avgHitting._2B = sumHitting._2B / vecHitters.size();
    avgHitting._3B = sumHitting._3B / vecHitters.size();
    avgHitting.HR = sumHitting.HR / vecHitters.size();
    avgHitting.R = sumHitting.R / vecHitters.size();
    avgHitting.RBI = sumHitting.RBI / vecHitters.size();
    avgHitting.BB = sumHitting.BB / vecHitters.size();
    avgHitting.SO = sumHitting.SO / vecHitters.size();
    avgHitting.HBP = sumHitting.HBP / vecHitters.size();
    avgHitting.SB = sumHitting.SB / vecHitters.size();
    avgHitting.CS = sumHitting.CS / vecHitters.size();
    avgHitting.AVG = sumHitting.H / sumHitting.AB;

    PerHitting stddevHitting;
    for (FBBPlayer* pHitter : vecHitters) {
        stddevHitting.PA += std::pow(pHitter->projection.hitting.PA - avgHitting.PA, 2.0);
        stddevHitting.AB += std::pow(pHitter->projection.hitting.AB - avgHitting.AB, 2.0);
        stddevHitting.H += std::pow(pHitter->projection.hitting.H - avgHitting.H, 2.0);
        stddevHitting._2B += std::pow(pHitter->projection.hitting._2B - avgHitting._2B, 2.0);
        stddevHitting._3B += std::pow(pHitter->projection.hitting._3B - avgHitting._3B, 2.0);
        stddevHitting.HR += std::pow(pHitter->projection.hitting.HR - avgHitting.HR, 2.0);
        stddevHitting.R += std::pow(pHitter->projection.hitting.R - avgHitting.R, 2.0);
        stddevHitting.RBI += std::pow(pHitter->projection.hitting.RBI - avgHitting.RBI, 2.0);
        stddevHitting.BB += std::pow(pHitter->projection.hitting.BB - avgHitting.BB, 2.0);
        stddevHitting.SO += std::pow(pHitter->projection.hitting.SO - avgHitting.SO, 2.0);
        stddevHitting.HBP += std::pow(pHitter->projection.hitting.HBP - avgHitting.HBP, 2.0);
        stddevHitting.SB += std::pow(pHitter->projection.hitting.SB - avgHitting.SB, 2.0);
        stddevHitting.CS += std::pow(pHitter->projection.hitting.CS - avgHitting.CS, 2.0);
        stddevHitting.AVG += std::pow(pHitter->projection.hitting.AVG() - avgHitting.AVG, 2.0);
    }

    stddevHitting.PA = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.PA);
    stddevHitting.AB = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.AB);
    stddevHitting.H = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.H);
    stddevHitting._2B = std::sqrt(1. / double(vecHitters.size()) * stddevHitting._2B);
    stddevHitting._3B = std::sqrt(1. / double(vecHitters.size()) * stddevHitting._3B);
    stddevHitting.HR = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.HR);
    stddevHitting.R = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.R);
    stddevHitting.RBI = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.RBI);
    stddevHitting.BB = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.BB);
    stddevHitting.SO = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.SO);
    stddevHitting.HBP = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.HBP);
    stddevHitting.SB = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.SB);
    stddevHitting.CS = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.CS);
    stddevHitting.AVG = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.AVG);

    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.zHitting.HR = (pHitter->projection.hitting.HR - avgHitting.HR) / stddevHitting.HR;
        pHitter->calculations.zHitting.R = (pHitter->projection.hitting.R - avgHitting.R) / stddevHitting.R;
        pHitter->calculations.zHitting.RBI = (pHitter->projection.hitting.RBI - avgHitting.RBI) / stddevHitting.RBI;
        pHitter->calculations.zHitting.SB = (pHitter->projection.hitting.SB - avgHitting.SB) / stddevHitting.SB;
        pHitter->calculations.zHitting.AVG = (pHitter->projection.hitting.AVG() - avgHitting.AVG) / stddevHitting.AVG;
    }

    sumHitting.AVG = 0;
    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.zHitting.AVG *= pHitter->projection.hitting.AB;
        sumHitting.AVG += pHitter->calculations.zHitting.AVG;
    }

    stddevHitting.AVG = 0;
    for (FBBPlayer* pHitter : vecHitters) {
        stddevHitting.AVG += std::pow(pHitter->calculations.zHitting.AVG - avgHitting.AVG, 2.0);
    }
    stddevHitting.AVG = std::sqrt(1. / double(vecHitters.size()) * stddevHitting.AVG);

    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.zHitting.AVG = (pHitter->calculations.zHitting.AVG - avgHitting.AVG) / stddevHitting.AVG;
    }

    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.zScore = 0;
        if (fbbApp->Settings()->categories.hitting.HR) {
            pHitter->calculations.zScore += pHitter->calculations.zHitting.HR;
        }
        if (fbbApp->Settings()->categories.hitting.R) {
            pHitter->calculations.zScore += pHitter->calculations.zHitting.R;
        }
        if (fbbApp->Settings()->categories.hitting.RBI) {
            pHitter->calculations.zScore += pHitter->calculations.zHitting.RBI;
        }
        if (fbbApp->Settings()->categories.hitting.SB) {
            pHitter->calculations.zScore += pHitter->calculations.zHitting.SB;
        }
        if (fbbApp->Settings()->categories.hitting.AVG) {
            pHitter->calculations.zScore += pHitter->calculations.zHitting.AVG;
        }
    }

    // Sort by zScore
    std::sort(vecHitters.begin(), vecHitters.end(), [](const FBBPlayer* pLHS, const FBBPlayer* pRHS) {
        return pLHS->calculations.zScore > pRHS->calculations.zScore;
    });

    // Get replacement player
    const size_t numDraftedHitters = fbbApp->Settings()->SumHitters() * fbbApp->Settings()->owners.size();
    if (numDraftedHitters >= vecHitters.size()) {
        return;
    }
    double replacementZ = vecHitters[numDraftedHitters]->calculations.zScore;
    
    // Normalize zScores
    double sumZ = 0;
    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.zScore -= replacementZ;
        if (pHitter->calculations.zScore > 0.0) {
            sumZ += pHitter->calculations.zScore;
        }
    }

    // Calculate cost estimates
    const double totalMoney = fbbApp->Settings()->leauge.budget * fbbApp->Settings()->owners.size();
    const double totalHittingMoney = fbbApp->Settings()->projections.hittingPitchingSplit * totalMoney;
    const double costPerZ = totalHittingMoney / sumZ;
    for (FBBPlayer* pHitter : vecHitters) {
        pHitter->calculations.estimate = pHitter->calculations.zScore * costPerZ;
    }
}

void FBBDraftBoardModel::CalculatePitchingZScores()
{
    std::vector<FBBPlayer*> vecPitchers = GetValidPitchers();

    struct PerPitching
    {
        double W = 0;
        double L = 0;
        double GS = 0;
        double G = 0;
        double SV = 0;
        double IP = 0;
        double H = 0;
        double ER = 0;
        double HR = 0;
        double SO = 0;
        double BB = 0;
        double ERA = 0;
        double WHIP = 0;
    };

    PerPitching sumPitching;
    for (FBBPlayer* pPitcher : vecPitchers) {
        sumPitching.W += pPitcher->projection.pitching.W;
        sumPitching.L += pPitcher->projection.pitching.L;
        sumPitching.GS += pPitcher->projection.pitching.GS;
        sumPitching.G += pPitcher->projection.pitching.G;
        sumPitching.SV += pPitcher->projection.pitching.SV;
        sumPitching.IP += pPitcher->projection.pitching.IP;
        sumPitching.H += pPitcher->projection.pitching.H;
        sumPitching.ER += pPitcher->projection.pitching.ER;
        sumPitching.HR += pPitcher->projection.pitching.HR;
        sumPitching.SO += pPitcher->projection.pitching.SO;
        sumPitching.BB += pPitcher->projection.pitching.BB;
    }

    PerPitching avgPitching;
    avgPitching.W = sumPitching.W / vecPitchers.size();
    avgPitching.L = sumPitching.L / vecPitchers.size();
    avgPitching.GS = sumPitching.GS / vecPitchers.size();
    avgPitching.G = sumPitching.G / vecPitchers.size();
    avgPitching.SV = sumPitching.SV / vecPitchers.size();
    avgPitching.IP = sumPitching.IP / vecPitchers.size();
    avgPitching.H = sumPitching.H / vecPitchers.size();
    avgPitching.ER = sumPitching.ER / vecPitchers.size();
    avgPitching.HR = sumPitching.HR / vecPitchers.size();
    avgPitching.SO = sumPitching.SO / vecPitchers.size();
    avgPitching.BB = sumPitching.BB / vecPitchers.size();
    avgPitching.ERA = (9 * sumPitching.ER) / (sumPitching.IP);
    avgPitching.WHIP = (sumPitching.BB + sumPitching.H) / sumPitching.IP;

    PerPitching stddevPitching;
    for (FBBPlayer* pPitcher : vecPitchers) {
        stddevPitching.W += std::pow(pPitcher->projection.pitching.W - avgPitching.W, 2.0);
        stddevPitching.L += std::pow(pPitcher->projection.pitching.L - avgPitching.L, 2.0);
        stddevPitching.GS += std::pow(pPitcher->projection.pitching.GS - avgPitching.GS, 2.0);
        stddevPitching.G += std::pow(pPitcher->projection.pitching.G - avgPitching.G, 2.0);
        stddevPitching.SV += std::pow(pPitcher->projection.pitching.SV - avgPitching.SV, 2.0);
        stddevPitching.IP += std::pow(pPitcher->projection.pitching.IP - avgPitching.IP, 2.0);
        stddevPitching.H += std::pow(pPitcher->projection.pitching.H - avgPitching.H, 2.0);
        stddevPitching.ER += std::pow(pPitcher->projection.pitching.ER - avgPitching.ER, 2.0);
        stddevPitching.HR += std::pow(pPitcher->projection.pitching.HR - avgPitching.HR, 2.0);
        stddevPitching.SO += std::pow(pPitcher->projection.pitching.SO - avgPitching.SO, 2.0);
        stddevPitching.BB += std::pow(pPitcher->projection.pitching.BB - avgPitching.BB, 2.0);
        stddevPitching.ERA += std::pow(pPitcher->projection.pitching.ERA() - avgPitching.ERA, 2.0);
        stddevPitching.WHIP += std::pow(pPitcher->projection.pitching.WHIP() - avgPitching.WHIP, 2.0);
    }

    stddevPitching.W = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.W);
    stddevPitching.L = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.L);
    stddevPitching.GS = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.GS);
    stddevPitching.G = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.G);
    stddevPitching.SV = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.SV);
    stddevPitching.IP = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.IP);
    stddevPitching.H = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.H);
    stddevPitching.ER = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.ER);
    stddevPitching.HR = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.HR);
    stddevPitching.SO = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.SO);
    stddevPitching.BB = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.BB);
    stddevPitching.ERA = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.ERA);
    stddevPitching.WHIP = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.WHIP);

    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.zPitching.W = (pPitcher->projection.pitching.W - avgPitching.W) / stddevPitching.W;
        pPitcher->calculations.zPitching.SV = (pPitcher->projection.pitching.SV - avgPitching.SV) / stddevPitching.SV;
        pPitcher->calculations.zPitching.SO = (pPitcher->projection.pitching.SO - avgPitching.SO) / stddevPitching.SO;
        pPitcher->calculations.zPitching.ERA = (pPitcher->projection.pitching.ERA() - avgPitching.ERA) / stddevPitching.ERA;
        pPitcher->calculations.zPitching.WHIP = (pPitcher->projection.pitching.WHIP() - avgPitching.WHIP) / stddevPitching.WHIP;
    }

    sumPitching.ERA = 0;
    sumPitching.WHIP = 0;
    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.zPitching.ERA *= pPitcher->projection.pitching.IP;
        pPitcher->calculations.zPitching.WHIP *= pPitcher->projection.pitching.IP;
        sumPitching.ERA += pPitcher->calculations.zPitching.ERA;
        sumPitching.WHIP += pPitcher->calculations.zPitching.WHIP;
    }

    stddevPitching.ERA = 0;
    stddevPitching.WHIP = 0;
    for (FBBPlayer* pPitcher : vecPitchers) {
        stddevPitching.ERA += std::pow(pPitcher->calculations.zPitching.ERA - avgPitching.ERA, 2.0);
        stddevPitching.WHIP += std::pow(pPitcher->calculations.zPitching.WHIP - avgPitching.WHIP, 2.0);
    }
    stddevPitching.ERA = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.ERA);
    stddevPitching.WHIP = std::sqrt(1. / double(vecPitchers.size()) * stddevPitching.WHIP);

    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.zPitching.ERA = -1. * (pPitcher->calculations.zPitching.ERA - avgPitching.ERA) / stddevPitching.ERA;
        pPitcher->calculations.zPitching.WHIP = -1. * (pPitcher->calculations.zPitching.WHIP - avgPitching.WHIP) / stddevPitching.WHIP;
    }

    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.zScore = 0;
        if (fbbApp->Settings()->categories.pitching.ERA) {
            pPitcher->calculations.zScore += pPitcher->calculations.zPitching.ERA;
        }
        if (fbbApp->Settings()->categories.pitching.SO) {
            pPitcher->calculations.zScore += pPitcher->calculations.zPitching.SO;
        }
        if (fbbApp->Settings()->categories.pitching.SV) {
            pPitcher->calculations.zScore += pPitcher->calculations.zPitching.SV;
        }
        if (fbbApp->Settings()->categories.pitching.W) {
            pPitcher->calculations.zScore += pPitcher->calculations.zPitching.W;
        }
        if (fbbApp->Settings()->categories.pitching.WHIP) {
            pPitcher->calculations.zScore += pPitcher->calculations.zPitching.WHIP;
        }
    }

    // Sort by zScore
    std::sort(vecPitchers.begin(), vecPitchers.end(), [](const FBBPlayer* pLHS, const FBBPlayer* pRHS) {
        return pLHS->calculations.zScore > pRHS->calculations.zScore;
    });

    // Get replacement player
    const size_t numDraftedPitchers = fbbApp->Settings()->SumPitchers() * fbbApp->Settings()->owners.size();
    if (numDraftedPitchers >= vecPitchers.size()) {
        return;
    }
    double replacementZ = vecPitchers[numDraftedPitchers]->calculations.zScore;

    // Normalize zScores
    double sumZ = 0;
    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.zScore -= replacementZ;
        if (pPitcher->calculations.zScore > 0.0) {
            sumZ += pPitcher->calculations.zScore;
        }
    }

    // Calculate cost estimates
    const double totalMoney = fbbApp->Settings()->leauge.budget * fbbApp->Settings()->owners.size();
    const double totalPitchingMoney = (1.0 - fbbApp->Settings()->projections.hittingPitchingSplit) * totalMoney;
    const double costPerZ = totalPitchingMoney / sumZ;
    for (FBBPlayer* pPitcher : vecPitchers) {
        pPitcher->calculations.estimate = pPitcher->calculations.zScore * costPerZ;
    }
}
