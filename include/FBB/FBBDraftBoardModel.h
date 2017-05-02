#pragma once

#include <QAbstractTableModel>

class FBBDraftBoardModel : public QAbstractTableModel
{
public:

    // Columns
    enum COLUMN
    {
        // General
        COLUMN_OWNER,
        COLUMN_PAID,
        COLUMN_NAME,
        COLUMN_TEAM,
        COLUMN_AGE,
        COLUMN_EXPERIENCE,
        COLUMN_POSITION,

        // Hitting
        COLUMN_FIRST_HITTING,
        COLUMN_AB = COLUMN_FIRST_HITTING,
        COLUMN_H,
        COLUMN_AVG,
        COLUMN_HR,
        COLUMN_R,
        COLUMN_RBI,
        COLUMN_SB,
        COLUMN_LAST_HITTING = COLUMN_SB,

        // Pitching
        COLUMN_FIRST_PITCHING,
        COLUMN_IP = COLUMN_FIRST_PITCHING,
        COLUMN_HA,
        COLUMN_BB,
        COLUMN_ER,
        COLUMN_SO,
        COLUMN_ERA,
        COLUMN_WHIP,
        COLUMN_W,
        COLUMN_SV,
        COLUMN_LAST_PITCHING = COLUMN_SV,

        // Fantasy
        COLUMN_Z,
        COLUMN_ESTIMATE,

        // Comment
        COLUMN_COMMENT,

        // Column count (must be last)
        COLUMN_COUNT,
    };

    enum
    {
        RawDataRole     = Qt::UserRole + 1,
        PrintFormatRole = Qt::UserRole + 2,
    };

    FBBDraftBoardModel(QObject* parent = nullptr);

    virtual int rowCount(const QModelIndex& parent) const override;
    virtual int columnCount(const QModelIndex& parent) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};