#pragma once

#include <QSortFilterProxyModel>
#include <QMap>

#include "PlayerTableModel.h"

//class OwnerSortFilterProxyModel : public QSortFilterProxyModel
//{
//public:
//
//    OwnerSortFilterProxyModel(uint32_t ownerId)
//        : m_ownerId(ownerId)
//    {
//    }
//
//    virtual bool filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const override
//    {
//        switch (sourceColumn)
//        {
//        case PlayerTableModel::COLUMN_DRAFT_POSITION:
//        case PlayerTableModel::COLUMN_NAME:
//        case PlayerTableModel::COLUMN_PAID:
//            return true;
//        default:
//            return false;
//        }
//    }
//
//    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override
//    {
//        auto AcceptOwner = [&]() -> bool
//        {
//            const QModelIndex& ownerIndex = sourceModel()->index(sourceRow, PlayerTableModel::COLUMN_OWNER, sourceParent);
//            uint32_t ownerId = sourceModel()->data(ownerIndex, PlayerTableModel::RawDataRole).toUInt();
//            return ownerId == m_ownerId;
//        };
//
//        return AcceptOwner();
//    }
//
//    uint32_t GetTotalSpent() const 
//    {
//        return m_sumCost;
//    }
//
//public slots:
//        
//    void OnDrafted(const DraftDialog::Results& results, const QModelIndex& index, QAbstractItemModel* model)
//    {
//        // Ignore other owners
//        if (results.ownerId != m_ownerId) {
//            return;
//        }
//
//        // Update position seen information
//        m_mapPositionsDrafted[results.position]++;
//
//        // Update sums
//        m_sumCost += results.cost;
//
//        // Player
//
//        // Invalidate filters
//        invalidate();
//    }
//
//private:
//
//    uint32_t m_ownerId = 0;
//    QMap<QString, uint32_t> m_mapPositionsDrafted;
//    uint32_t m_sumCost = 0;
//
//};