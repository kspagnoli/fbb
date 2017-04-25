#pragma once

#include <QAbstractTableModel>

class FBBDraftBoardModel : public QAbstractTableModel
{
public:

    // Columns
    enum COLUMN
    {
        // General
        COLUMN_FLAG,
        COLUMN_RANK,
        COLUMN_DRAFT_BUTTON,
        COLUMN_OWNER,
        COLUMN_PAID,
        COLUMN_NAME,
        COLUMN_TEAM,
        COLUMN_AGE,
        COLUMN_EXPERIENCE,
        COLUMN_CATERGORY,
        COLUMN_POSITION,

        // Hitting
        COLUMN_AB,
        COLUMN_H,
        COLUMN_AVG,
        COLUMN_HR,
        COLUMN_R,
        COLUMN_RBI,
        COLUMN_SB,

        // Pitching
        COLUMN_IP,
        COLUMN_HA,
        COLUMN_BB,
        COLUMN_ER,
        COLUMN_SO,
        COLUMN_ERA,
        COLUMN_WHIP,
        COLUMN_W,
        COLUMN_SV,

        // Fantasy
        COLUMN_Z,
        COLUMN_ESTIMATE,

        // Link
        COLUMN_ID_LINK,

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