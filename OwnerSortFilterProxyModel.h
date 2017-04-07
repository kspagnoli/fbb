#pragma once

#include <QSortFilterProxyModel>

#include "PlayerTableModel.h"

class OwnerSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    OwnerSortFilterProxyModel(uint32_t ownerId, class QAbstractTableModel* sourceModel, QObject* parent);

    // QSortFilterProxyModel interface
    virtual bool filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const override;
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

    float Sum(enum PlayerTableModel::COLUMN column) const;
    float Count(const Player::CatergoryMask& catergory) const;
    float AVG() const;
    float ERA() const;
    float WHIP() const;
    int32_t GetRemainingBudget() const;
    int32_t GetRosterSlotsFilled() const;
    int32_t GetRosterSpotsToFill() const;
    int32_t GetMaxBid() const;
    int32_t GetValue() const;

private:
    uint32_t m_ownerId = 0;
};