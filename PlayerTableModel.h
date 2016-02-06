#pragma once

#include "PlayerAppearances.h"
#include "Player.h"
#include "DraftDelegate.h"

#include <QAbstractTableModel>

class PlayerApperances;

// Helpers
QString PositionToString(const PlayerPosition& position);
QStringList PositionMaskToStringList(const PlayerPositionBitfield& positionBitField);

PlayerPosition StringToPosition(const QString& position);

class PlayerTableModel : public QAbstractTableModel
{

    Q_OBJECT

public:

    // Columns
    enum COLUMN
    {
        // General
        COLUMN_RANK,
        COLUMN_DRAFT_BUTTON,
        COLUMN_OWNER,
        COLUMN_DRAFT_POSITION,
        COLUMN_PAID,
        COLUMN_ID,
        COLUMN_NAME,
        COLUMN_TEAM,
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

        // General
        COLUMN_Z,
        COLUMN_ESTIMATE,
        COLUMN_COMMENT,

        COLUMN_COUNT,
    };

    // Constructor
    PlayerTableModel(QObject* parent);

    // Load projections
    void LoadHittingProjections(const std::string& filename, const PlayerApperances& playerApperances);
    void LoadPitchingProjections(const std::string& filename, const PlayerApperances& playerApperances);

    // Save status
    bool SaveDraftStatus(const QString& filename) const;
    bool LoadDraftStatus(const QString& filename);

    // Called after loaded projections to initialize target values
    void InitializeTargetValues();

    // Get per-stat target values (e.g. 3rd place)
    float GetTargetValue(enum COLUMN stat) const;

    // QAbstractTableModel interfaces
    virtual int rowCount(const QModelIndex &) const override;
    virtual int columnCount(const QModelIndex &) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // Debug helper
    void DraftRandom();

    // Data roles
    static const int RawDataRole = Qt::UserRole + 1;
    static const int ChartFormatRole = Qt::UserRole + 2;

signals:

    void DraftedBegin();
    void Drafted(const DraftDialog::Results& results, const QModelIndex& index, QAbstractItemModel* model);
    void DraftedEnd();

private:

    // Update model
    friend class DraftDelegate;
    void OnDrafted(const DraftDialog::Results& results, const QModelIndex& index, QAbstractItemModel* model);

    // Hitter data model
    std::vector<Player> m_vecPlayers;

    // Inflation factors
    double m_inflationFactor = 1.0;

    // Per-stat target values
    std::array<float, size_t(COLUMN_COUNT)> m_vecTargetValues;
};
