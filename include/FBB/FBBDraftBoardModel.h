#pragma once

#include <QAbstractTableModel>
#include <QFont>
#include <QJsonObject>

#include "FBB/FBBPlayer.h"

class FBBDraftBoardModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    // Columns
    enum COLUMN
    {
        // Key
        COLUMN_ID,

        // General
        COLUMN_RANK,
        COLUMN_OWNER,
        COLUMN_PAID,
        COLUMN_NAME,
        COLUMN_TEAM,
        COLUMN_AGE,
        COLUMN_EXPERIENCE,
        COLUMN_POSITION,

        // Hitting
        COLUMN_FIRST_HITTING_BASE,
        COLUMN_AB = COLUMN_FIRST_HITTING_BASE,
        COLUMN_H,
        COLUMN_LAST_HITTING_BASE = COLUMN_H,
        
        // Hitting stat
        COLUMN_FIRST_HITTING,
        COLUMN_AVG = COLUMN_FIRST_HITTING,
        COLUMN_HR,
        COLUMN_R,
        COLUMN_RBI,
        COLUMN_SB,
        COLUMN_LAST_HITTING = COLUMN_SB,

        // Hitting z-state
        COLUMN_FIRST_Z_HITTING,
        COLUMN_Z_AVG = COLUMN_FIRST_Z_HITTING,
        COLUMN_Z_HR,
        COLUMN_Z_R,
        COLUMN_Z_RBI,
        COLUMN_Z_SB,
        COLUMN_LAST_Z_HITTING = COLUMN_Z_SB,

        // Pitching
        COLUMN_FIRST_PITCHING_BASE,
        COLUMN_IP = COLUMN_FIRST_PITCHING_BASE,
        COLUMN_HA,
        COLUMN_BB,
        COLUMN_ER,
        COLUMN_LAST_PITCHING_BASE = COLUMN_ER,

        // Pitching stats
        COLUMN_FIRST_PITCHING,
        COLUMN_SO = COLUMN_FIRST_PITCHING,
        COLUMN_ERA,
        COLUMN_WHIP,
        COLUMN_W,
        COLUMN_SV,
        COLUMN_LAST_PITCHING = COLUMN_SV,

        // Pitching z-stats
        COLUMN_FIRST_Z_PITCHING,
        COLUMN_Z_SO = COLUMN_FIRST_Z_PITCHING,
        COLUMN_Z_ERA,
        COLUMN_Z_WHIP,
        COLUMN_Z_W,
        COLUMN_Z_SV,
        COLUMN_LAST_Z_PITCHING = COLUMN_Z_SV,

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

    void Reset(const std::vector<FBBPlayer*>& vecPlayers);
    uint32_t PlayerCount() const;
    void AddPlayer(FBBPlayer* pPlayer);
    FBBPlayer* GetPlayer(uint32_t index);
    std::vector<FBBPlayer*> GetValidHitters();
    std::vector<FBBPlayer*> GetValidPitchers();
    
    // Export
    QJsonObject ToJson() const;

    // QAbstractTableModel
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex& parent) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    
signals:
    void PlayerDrafted(FBBPlayer* player);

private:

    void CalculateZScores();
    void CalculateHittingZScores();
    void CalculatePitchingZScores();

    std::vector<FBBPlayer*> m_vecPlayers;

    QFont m_font;
    QFont m_draftedFont;
};