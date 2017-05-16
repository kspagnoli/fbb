#pragma once

#include "FBB/FBBPlayer.h"

#include <vector>
#include <QAbstractTableModel>

class FBBDraftLogModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    static FBBDraftLogModel& Instance();

    // QAbstractTableModel interface
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    FBBDraftLogModel(QObject* pParent);
    std::vector<FBBPlayer*> m_vecDraftedPlayers;
};