#pragma once

#include "Pitcher.h"
#include <QAbstractTableModel>

class PitcherTableModel : public QAbstractTableModel
{

public:

    // Columns
    enum COLUMN
    {
        COLUMN_RANK,
        COLUMN_NAME,
        COLUMN_TEAM,
        COLUMN_POSITION,
        COLUMN_IP,
        COLUMN_SO,
        COLUMN_ERA,
        COLUMN_WHIP,
        COLUMN_W,
        COLUMN_SV,
        COLUMN_Z,
        COLUMN_ESTIMATE,
        COLUMN_COMMENT,

        COLUMN_COUNT
    };

    // Constructor
    PitcherTableModel(const std::string& filename, QObject* parent);

    // QAbstractTableModel interfaces
    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // Data roles
    static const int RawDataRole = Qt::UserRole + 1;

private:

    // Hitter data model
    std::vector<Pitcher> m_vecPitchers;
};
