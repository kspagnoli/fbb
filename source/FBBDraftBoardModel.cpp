#include "FBB/FBBDraftBoardModel.h"
#include "FBB/FBBPlayerDataService.h"
#include "FBB/FBBProjectionService.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"
#include "FBB/FBBPositionService.h"

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

    // Listen for settings changed
    connect(&FBBProjectionService::Instance(), &FBBProjectionService::BeginProjectionsUpdated, this, [=]() {
        beginResetModel();
    });

    // Listen for settings changed
    connect(&FBBProjectionService::Instance(), &FBBProjectionService::EndProjectionsUpdated, this, [=]() {
        endResetModel();
    });

    // Bootstrap position data
    FBBPositionService::LoadPositionData();

    // 
    FBBPlayerDataService::SetDraftModel(this);
}

QJsonObject FBBDraftBoardModel::ToJson() const
{
    QJsonObject jsonArray;
    FBBPlayerDataService::ForEach([&](const FBBPlayer& player){

        if (player.draftInfo.owner != -1)
        {
            QVariantMap vm;
            vm["owner"] = player.draftInfo.owner;
            vm["paid"] = player.draftInfo.paid;
            vm["position"] = player.draftInfo.position;
            jsonArray[player.id] = (QJsonObject::fromVariantMap(vm));
        }
    });

    QJsonObject json;
    json["drafted"] = jsonArray;
    return json;
}

int FBBDraftBoardModel::rowCount(const QModelIndex& parent) const
{
    return FBBPlayerDataService::PlayerCount();
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
    const FBBPlayer* pPlayer = FBBPlayerDataService::GetPlayer(index.row());

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
            return FBBLeaugeSettings::Instance().owners[pPlayer->draftInfo.owner]->abbreviation;
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
    emit dataChanged(index(0, COLUMN_FIRST_HITTING), index(FBBPlayerDataService::PlayerCount() - 1, COLUMN_LAST_PITCHING), roles);
}
