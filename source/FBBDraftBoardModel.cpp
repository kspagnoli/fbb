#include "FBB/FBBDraftBoardModel.h"
#include "FBB/FBBPlayerDataService.h"
#include "FBB/FBBProjectionService.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"
#include "FBB/FBBPositionService.h"

namespace Impl {

    template <typename T>
    QVariant FormatType(T x)
    {
        return x;
    }

    QVariant FormatType(float x)
    {
        QString str;
        str.setNum(x, 'f', 3);
        return str;
    }

    template <typename Fn_Getter>
    QVariant GetProjectionData(int role, const std::unique_ptr<FBBPlayer::Projection>& sPtr, FBBPlayer::Projection::Type type, const Fn_Getter& fn)
    {
        if (sPtr->type != type) {
            return QVariant();
        }

        if (role == FBBDraftBoardModel::RawDataRole) {
            if (!sPtr) {
                return -1;
            } else {
                return fn();
            }
        } else if (role == Qt::DisplayRole) {
            if (!sPtr) {
                return "N/A";
            } else {
                return FormatType(fn());
            }
        }
        return QVariant();
    }

}

#define GET_PROJECTION_DATA(role, PTR, TYPE, STAT)          \
    return Impl::GetProjectionData(role, PTR, TYPE, [=]{    \
        return PTR->STAT;                                   \
    });

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
        case COLUMN_OWNER:
            return "Owner";
        case COLUMN_PAID:
            return "Paid";
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
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_HITTING, hitting.AB);
        case COLUMN_H:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_HITTING, hitting.H);
        case COLUMN_AVG:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_HITTING, hitting.AVG());
        case COLUMN_HR:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_HITTING, hitting.HR);
        case COLUMN_R:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_HITTING, hitting.R);
        case COLUMN_RBI:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_HITTING, hitting.RBI);
        case COLUMN_SB:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_HITTING, hitting.SB);
        case COLUMN_IP:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_PITCHING, pitching.IP);
        case COLUMN_HA:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_PITCHING, pitching.H);
        case COLUMN_BB:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_PITCHING, pitching.BB);
        case COLUMN_ER:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_PITCHING, pitching.ER);
        case COLUMN_SO:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_PITCHING, pitching.SO);
        case COLUMN_ERA:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_PITCHING, pitching.ERA());
        case COLUMN_WHIP:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_PITCHING, pitching.WHIP());
        case COLUMN_W:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_PITCHING, pitching.W);
        case COLUMN_SV:
            GET_PROJECTION_DATA(role, pPlayer->spProjection, FBBPlayer::Projection::PROJECTION_TYPE_PITCHING, pitching.SV);
        case COLUMN_ESTIMATE:
            return "$";
        case COLUMN_Z:
            return pPlayer->calculations.zScore;
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

