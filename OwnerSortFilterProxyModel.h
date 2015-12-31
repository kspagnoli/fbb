#pragma once

#include <QSortFilterProxyModel>
#include <QMap>

#include "PlayerTableModel.h"

class OwnerSortFilterProxyModel : public QSortFilterProxyModel
{
public:

    OwnerSortFilterProxyModel(uint32_t ownerId)
        : m_ownerId(ownerId)
    {
    }

    virtual bool filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const override
    {
        switch (sourceColumn)
        {
        case PlayerTableModel::COLUMN_DRAFT_POSITION:
        case PlayerTableModel::COLUMN_NAME:
        case PlayerTableModel::COLUMN_PAID:
            return true;
        default:
            return false;
        }
    }

    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override
    {
        auto AcceptDummy = [&]() -> bool
        {
            // Get dummy count
            const QModelIndex& dummyIndex = sourceModel()->index(sourceRow, PlayerTableModel::COLUMN_DUMMY, sourceParent);
            uint32_t dummy = sourceModel()->data(dummyIndex, PlayerTableModel::RawDataRole).toUInt();

            // Reject non-dummy values
            if (dummy == 0) {
                return false;
            }
    
            // Get dummy draft position
            const QModelIndex& draftPositionIndex = sourceModel()->index(sourceRow, PlayerTableModel::COLUMN_DRAFT_POSITION, sourceParent);
            QString dummyPosition = sourceModel()->data(draftPositionIndex, PlayerTableModel::RawDataRole).toString();

            // Reject if we've seen 
            return dummy > m_mapPositionsDrafted[dummyPosition];
        };

        auto AcceptOwner = [&]() -> bool
        {
            const QModelIndex& ownerIndex = sourceModel()->index(sourceRow, PlayerTableModel::COLUMN_OWNER, sourceParent);
            uint32_t ownerId = sourceModel()->data(ownerIndex, PlayerTableModel::RawDataRole).toUInt();
            return ownerId == m_ownerId;
        };

        return AcceptDummy() || AcceptOwner();
    }

public slots:
        
    void OnDrafted(const DraftDialog::Results& results, const QModelIndex& index, QAbstractItemModel* model)
    {
        // Ignore other owners
        if (results.ownerId != m_ownerId) {
            return;
        }

        // Update position seen information
        m_mapPositionsDrafted[results.position]++;

        // Update sums
        m_sumCost += results.cost;

        // Invalidate filters
        invalidate();
    }

private:

    uint32_t m_ownerId;
    QMap<QString, uint32_t> m_mapPositionsDrafted;
    uint32_t m_sumCost;
};