#pragma once

#include "Hitter.h"
#include <QAbstractTableModel>

class HitterTableModel : public QAbstractTableModel
{

public:

    // Columns
    enum COLUMN
    {
        COLUMN_RANK,
        COLUMN_NAME,
        COLUMN_TEAM,
        COLUMN_POSITION,
        COLUMN_AB,
        COLUMN_AVG,
        COLUMN_HR,
        COLUMN_R,
        COLUMN_RBI,
        COLUMN_SB,
        COLUMN_Z,
        COLUMN_ESTIMATE,
        COLUMN_COMMENT,

        COLUMN_COUNT
    };

    // Constructor
    HitterTableModel(const std::string& filename, QObject* parent);

    // QAbstractTableModel interfaces
    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:

    // Hitter data model
    std::vector<Hitter> m_vecHitters;
};
