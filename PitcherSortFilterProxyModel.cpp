#include "PitcherSortFilterProxyModel.h"
#include "PitcherTableModel.h"

//------------------------------------------------------------------------------
// PitcherSortFilterProxyModel
//------------------------------------------------------------------------------
PitcherSortFilterProxyModel::PitcherSortFilterProxyModel()
{
}

//------------------------------------------------------------------------------
// lessThan (override)
//------------------------------------------------------------------------------
bool PitcherSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    const QVariant& leftData = sourceModel()->data(left);
    const QVariant& rightData = sourceModel()->data(right);

    return leftData < rightData;
}

//------------------------------------------------------------------------------
// filterAcceptsRow (override)
//------------------------------------------------------------------------------
bool PitcherSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    const QModelIndex& abIndex = sourceModel()->index(sourceRow, PitcherTableModel::COLUMN_IP, sourceParent);
    float ab = sourceModel()->data(abIndex).toFloat();
    return m_filterActive ? ab > 200 : ab > 10;
}

//------------------------------------------------------------------------------
// OnFilterAvailable
//------------------------------------------------------------------------------
void PitcherSortFilterProxyModel::OnFilterAvailable(bool checked)
{
    m_filterActive = checked;
    QSortFilterProxyModel::filterChanged();
}


