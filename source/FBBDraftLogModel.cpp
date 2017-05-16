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
    return 1;
}

QVariant FBBDraftLogModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole) {
        return m_vecDraftedPlayers[index.row()]->name;
    }

    return QVariant();
}

QVariant FBBDraftLogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        switch (section)
        {
        case 0:
            return "Player";
        default:
            break;
        }
    }

    return QVariant();
}