#pragma once

#include <QSortFilterProxyModel>
#include <QMap>

#include "PlayerTableModel.h"

class OwnerSortFilterProxyModel : public QSortFilterProxyModel
{
public:

    OwnerSortFilterProxyModel(uint32_t ownerId, QAbstractTableModel* sourceModel, QObject* parent)
        : m_ownerId(ownerId)
        , QSortFilterProxyModel(parent)
    {
        setSourceModel(sourceModel);
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
        auto AcceptOwner = [&]() -> bool
        {
            const QModelIndex& ownerIndex = sourceModel()->index(sourceRow, PlayerTableModel::COLUMN_OWNER, sourceParent);
            uint32_t ownerId = sourceModel()->data(ownerIndex, PlayerTableModel::RawDataRole).toUInt();
            return ownerId == m_ownerId;
        };
        
        return AcceptOwner();

        return true;
    }

    virtual QVariant data(const QModelIndex& index, int role) const override
    {
        // Gray out taken players
        if (role == Qt::TextColorRole) {
            return QVariant();
        }

        return sourceModel()->data(QSortFilterProxyModel::mapToSource(index), role);
    }

    virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const override
    {
        const QVariant& leftData = sourceModel()->data(left, PlayerTableModel::RawDataRole);
        const QVariant& rightData = sourceModel()->data(right, PlayerTableModel::RawDataRole);
        return leftData < rightData;
    }

    float Sum(enum PlayerTableModel::COLUMN column) const
    {
        float sum = 0;
        for (auto i = 0; i < rowCount(); i++) {
            auto idx = mapToSource(index(i, 0));
            auto srcIdx = sourceModel()->index(idx.row(), column);
            sum += sourceModel()->data(srcIdx, PlayerTableModel::RawDataRole).toFloat();
        }
        return sum;
    }

    float Count(const Player::CatergoryMask& catergory) const
    {
        uint32_t count = 0;
        for (auto i = 0; i < rowCount(); i++) {
            auto idx = mapToSource(index(i, 0));
            auto srcIdx = sourceModel()->index(idx.row(), PlayerTableModel::COLUMN_CATERGORY);
            if (catergory & sourceModel()->data(srcIdx, PlayerTableModel::RawDataRole).toUInt()) {
                count++;
            }
        }
        return count;
    }

public slots:

    void OnDrafted(const DraftDialog::Results& results, const QModelIndex& index, QAbstractItemModel* model)
    {
        // Ignore other owners
        if (results.ownerId != m_ownerId) {
            return;
        }

        // Invalidate filters
        QSortFilterProxyModel::invalidate();

        // Update table view
        emit dataChanged(index, index);
    }

private:

    uint32_t m_ownerId = 0;
};