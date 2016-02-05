#pragma once

#include <QAbstractTableModel>
#include <vector>

class PlayerTableModel;

//------------------------------------------------------------------------------
// SummaryItemModel
//------------------------------------------------------------------------------
class SummaryTableModel : public QAbstractTableModel
{

public:

    SummaryTableModel(const std::vector<class OwnerSortFilterProxyModel*>& vecOwnerSortFilterProxyModel, PlayerTableModel* playerTableModel, QWidget* parent);

    enum RankRows
    {
        RANK,
        TEAM,
        BUDGET,
        ROSTER_SIZE,
        BA,
        R,
        HR,
        RBI,
        SB,
        ERA,
        WHIP,
        W,
        K,
        S,
        SUM,
        COUNT
    };

    enum DataRoles
    {
        RawDataRole    = Qt::UserRole + 0,
        RankRole       = Qt::UserRole + 1,
    };

    virtual int rowCount(const QModelIndex& index) const override;
    virtual int columnCount(const QModelIndex& index) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:

    void OnDraftedEnd();

private:

    struct OwnerPoints
    {
        // Hitting (ranks)
        uint32_t rankAVG = 0;
        uint32_t rankR = 0;
        uint32_t rankHR = 0;
        uint32_t rankRBI = 0;
        uint32_t rankSB = 0;

        // Pitching (ranks)
        uint32_t rankERA = 0;
        uint32_t rankWHIP = 0;
        uint32_t rankW = 0;
        uint32_t rankK = 0;
        uint32_t rankSV = 0;

        // Summary
        uint32_t SUM = 0;
        uint32_t rankSUM = 0;
    };

    std::vector<class OwnerSortFilterProxyModel*> m_vecOwnerSortFilterProxyModels;
    PlayerTableModel* m_playerTableModel;
    std::vector<OwnerPoints> m_vecOwnerPoints;
};