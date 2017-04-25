#include "FBB/FBBDraftBoardSortFilterProxyModel.h"
#include "FBB/FBBDraftBoardModel.h"

FBBDraftBoardSortFilterProxyModel::FBBDraftBoardSortFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

bool FBBDraftBoardSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    const QVariant& leftData = sourceModel()->data(left, FBBDraftBoardModel::RawDataRole);
    const QVariant& rightData = sourceModel()->data(right, FBBDraftBoardModel::RawDataRole);
    return leftData < rightData;
}

bool FBBDraftBoardSortFilterProxyModel::filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const
{
    return true;
}

bool FBBDraftBoardSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    return true;
}