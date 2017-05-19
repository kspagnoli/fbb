#include "FBB/FBBDraftLogModel.h"
#include "FBB/FBBPlayerDataService.h"

#include <QApplication>

FBBDraftLogModel& FBBDraftLogModel::Instance()
{
    static FBBDraftLogModel* s_instance = new FBBDraftLogModel(qApp);
    return *s_instance;
}

FBBDraftLogModel::FBBDraftLogModel(QObject* pParent)
    : QAbstractTableModel(pParent)
{
    connect(&FBBPlayerDataService::Instance(), &FBBPlayerDataService::PlayerDrafted, this, [=](FBBPlayer* pPlayer) {
        beginResetModel();
        m_vecDraftedPlayers.push_back(pPlayer);
        endResetModel();
    });
}

int FBBDraftLogModel::rowCount(const QModelIndex& parent) const
{
    return static_cast<int>(m_vecDraftedPlayers.size());
}

int FBBDraftLogModel::columnCount(const QModelIndex& parent) const
{
    return DRAFTLOG_COLUMN_COUNT;
}

QVariant FBBDraftLogModel::data(const QModelIndex& index, int role) const
{
    const FBBPlayer* pPlayer = m_vecDraftedPlayers[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column())
        {
        case DRAFTLOG_ID:
            return index.row();
        case DRAFTLOG_NAME:
            return pPlayer->name;
        case DRAFTLOG_OWNER:
            return pPlayer->draftInfo.owner;
        case DRAFTLOG_PRICE:
            return pPlayer->draftInfo.paid;
        default:
            break;
        }
    }

    return QVariant();
}

QVariant FBBDraftLogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        switch (section)
        {
        case DRAFTLOG_ID:
            return "#";
        case DRAFTLOG_NAME:
            return "Player";
        case DRAFTLOG_OWNER:
            return "Owner";
        case DRAFTLOG_PRICE:
            return "$";
        default:
            break;
        }
    }

    return QVariant();
}