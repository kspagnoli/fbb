#pragma once

#include <QSortFilterProxyModel>

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
        const QModelIndex& ownerIndex = sourceModel()->index(sourceRow, PlayerTableModel::COLUMN_OWNER, sourceParent);
        uint32_t ownerId = sourceModel()->data(ownerIndex, PlayerTableModel::RawDataRole).toInt();
        return (ownerId == m_ownerId);
    }

private:

    uint32_t m_ownerId;

};