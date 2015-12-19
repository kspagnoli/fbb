#pragma once

#include "Pitcher.h"
#include <QAbstractTableModel>

class PlayerApperances;

class PitcherTableModel : public QAbstractTableModel
{

public:

    // Columns
    enum COLUMN
    {
        COLUMN_RANK,
        COLUMN_DRAFT_STATUS,
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
    PitcherTableModel(const std::string& filename, const PlayerApperances& appearances, QObject* parent);

    // QAbstractTableModel interfaces
    virtual int rowCount(const QModelIndex &) const override;
    virtual int columnCount(const QModelIndex &) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // Data roles
    static const int RawDataRole = Qt::UserRole + 1;

private:

    // Hitter data model
    std::vector<Pitcher> m_vecPitchers;
};
