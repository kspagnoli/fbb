#include "FBB/FBBDraftBoardModel.h"
#include "FBB/FBBPlayerDataService.h"
#include "FBB/FBBProjectionService.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"
#include "FBB/FBBPositionService.h"

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
}

int FBBDraftBoardModel::rowCount(const QModelIndex& parent) const
{
    return FBBPlayerDataService::PlayerCount();
}

int FBBDraftBoardModel::columnCount(const QModelIndex& parent) const
{
    return COLUMN_COUNT;
}

QVariant FBBDraftBoardModel::data(const QModelIndex& index, int role) const
{
    // Get player
    const FBBPlayer* pPlayer = FBBPlayerDataService::GetPlayer(index.row());

    // Sanity check
    if (!pPlayer) {
        return QVariant();
    }

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
            return FBBPositionMaskToString(pPlayer->eligablePositions);
        case COLUMN_AB:
             return ToQVariant(pPlayer->spProjection->hitting.AB, role);
        case COLUMN_H:
             return ToQVariant(pPlayer->spProjection->hitting.H, role);
        case COLUMN_AVG:
            if (m_mode == Mode::STAT) {
                return ToQVariant(pPlayer->spProjection->hitting.AVG(), role);
            } else if (m_mode == Mode::Z_SCORE) {
                return ToQVariant(pPlayer->calculations.zHitting.AVG, role);
            }
            break;
        case COLUMN_HR:
            if (m_mode == Mode::STAT) {
                return ToQVariant(pPlayer->spProjection->hitting.HR, role);
            } else if (m_mode == Mode::Z_SCORE) {
                return ToQVariant(pPlayer->calculations.zHitting.HR, role);
            }
            break;
        case COLUMN_R:
            if (m_mode == Mode::STAT) {
                return ToQVariant(pPlayer->spProjection->hitting.R, role);
            } else if (m_mode == Mode::Z_SCORE) {
                return ToQVariant(pPlayer->calculations.zHitting.R, role);
            }
            break;
        case COLUMN_RBI:
            if (m_mode == Mode::STAT) {
                return ToQVariant(pPlayer->spProjection->hitting.RBI, role);
            } else if (m_mode == Mode::Z_SCORE) {
                return ToQVariant(pPlayer->calculations.zHitting.RBI, role);
            }
            break;
        case COLUMN_SB:
            if (m_mode == Mode::STAT) {
                return ToQVariant(pPlayer->spProjection->hitting.SB, role);
            } else if (m_mode == Mode::Z_SCORE) {
                return ToQVariant(pPlayer->calculations.zHitting.SB, role);
            }
            break;
        case COLUMN_IP:
            return ToQVariant(pPlayer->spProjection->pitching.IP, role);
        case COLUMN_HA:
            return ToQVariant(pPlayer->spProjection->pitching.H, role);
        case COLUMN_BB:
            return ToQVariant(pPlayer->spProjection->pitching.BB, role);
        case COLUMN_ER:
            return ToQVariant(pPlayer->spProjection->pitching.ER, role);
        case COLUMN_SO:
            if (m_mode == Mode::STAT) {
                return ToQVariant(pPlayer->spProjection->pitching.SO, role);
            } else if (m_mode == Mode::Z_SCORE) {
                return ToQVariant(pPlayer->calculations.zPitching.SO, role);
            }
            break;
        case COLUMN_ERA:
            if (m_mode == Mode::STAT) {
                return ToQVariant(pPlayer->spProjection->pitching.ERA(), role);
            } else if (m_mode == Mode::Z_SCORE) {
                return ToQVariant(pPlayer->calculations.zPitching.ERA, role);
            }
            break;
        case COLUMN_WHIP:
            if (m_mode == Mode::STAT) {
                return ToQVariant(pPlayer->spProjection->pitching.WHIP(), role);
            } else if (m_mode == Mode::Z_SCORE) {
                return ToQVariant(pPlayer->calculations.zPitching.WHIP, role);
            }
            break;
        case COLUMN_W:
            if (m_mode == Mode::STAT) {
                return ToQVariant(pPlayer->spProjection->pitching.W, role);
            } else if (m_mode == Mode::Z_SCORE) {
                return ToQVariant(pPlayer->calculations.zPitching.W, role);
            }
            break;
        case COLUMN_SV:
            if (m_mode == Mode::STAT) {
                return ToQVariant(pPlayer->spProjection->pitching.SV, role);
            } else if (m_mode == Mode::Z_SCORE) {
                return ToQVariant(pPlayer->calculations.zPitching.SV, role);
            }
        case COLUMN_ESTIMATE:
            return ToQVariant(pPlayer->calculations.estimate, role);
        case COLUMN_Z: {
            return ToQVariant(pPlayer->calculations.zScore, role);
        } break;
        case COLUMN_COMMENT:
            return "Comment";
        }
    }

    return QVariant();
}

QVariant FBBDraftBoardModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {

        if (role == Qt::InitialSortOrderRole) {
            return section;
        }

        if (role == Qt::DisplayRole) {

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
