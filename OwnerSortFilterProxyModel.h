#pragma once

#include <QSortFilterProxyModel>

#include "HitterTableModel.h"

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
        case HitterTableModel::COLUMN_DRAFT_POSITION:
        case HitterTableModel::COLUMN_NAME:
        case HitterTableModel::COLUMN_PAID:
            return true;
        default:
            return false;
        }
    }

    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override
    {
        const QModelIndex& ownerIndex = sourceModel()->index(sourceRow, HitterTableModel::COLUMN_OWNER, sourceParent);
        uint32_t ownerId = sourceModel()->data(ownerIndex, HitterTableModel::RawDataRole).toInt();
        return (ownerId == m_ownerId);
    }

private:

    uint32_t m_ownerId;

};