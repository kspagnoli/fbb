#include "HitterSortFilterProxyModel.h"
#include "HitterTableModel.h"

//------------------------------------------------------------------------------
// HitterSortFilterProxyModel
//------------------------------------------------------------------------------
HitterSortFilterProxyModel::HitterSortFilterProxyModel()
{
}

//------------------------------------------------------------------------------
// lessThan (override)
//------------------------------------------------------------------------------
bool HitterSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    const QVariant& leftData = sourceModel()->data(left);
    const QVariant& rightData = sourceModel()->data(right);

    return leftData < rightData;
}

//------------------------------------------------------------------------------
// filterAcceptsRow (override)
//------------------------------------------------------------------------------
bool HitterSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    const QModelIndex& abIndex = sourceModel()->index(sourceRow, HitterTableModel::COLUMN_AB, sourceParent);
    float ab = sourceModel()->data(abIndex).toFloat();
    return m_filterActive ? ab > 200 : ab > 10;
}

//------------------------------------------------------------------------------
// OnFilterAvailable
//------------------------------------------------------------------------------
void HitterSortFilterProxyModel::OnFilterAvailable(bool checked)
{
    m_filterActive = checked;
    QSortFilterProxyModel::filterChanged();
}


