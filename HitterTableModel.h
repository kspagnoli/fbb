#pragma once

#include "PlayerAppearances.h"
#include "Hitter.h"
#include <QAbstractTableModel>

class PlayerApperances;

class HitterTableModel : public QAbstractTableModel
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
    HitterTableModel(const std::string& filename, const PlayerApperances& playerApperances, QObject* parent);

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
    std::vector<Hitter> m_vecHitters;
};
