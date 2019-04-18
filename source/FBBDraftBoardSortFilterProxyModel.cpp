#include "FBB/FBBDraftBoardSortFilterProxyModel.h"
#include "FBB/FBBDraftBoardModel.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"

FBBDraftBoardSortFilterProxyModel::FBBDraftBoardSortFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    connect(fbbApp->Settings(), &FBBLeaugeSettings::SettingsChanged, this, [=]() {
        invalidateFilter();
    });
}
void FBBDraftBoardSortFilterProxyModel::SetShowDrafted(bool enable)
{
    m_showDrafted = enable;
    emit invalidateFilter();
}

void FBBDraftBoardSortFilterProxyModel::SetPositionFilter(FBBPositionMask mask)
{
    m_positionFilter = mask;
    emit invalidateFilter();
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
    const FBBPlayer* pPlayer = static_cast<FBBDraftBoardModel*>(sourceModel())->GetPlayer(sourceRow);

    if (!pPlayer->IsValidUnderCurrentSettings()) {
        return false;
    }

    if (!m_showDrafted && pPlayer->draftInfo.owner)
    {
        return false;
    }

    if (m_positionFilter & pPlayer->EligablePositions())
    {
        return true;
    }

    return false;
}
