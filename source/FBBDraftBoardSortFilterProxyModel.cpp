#include "FBB/FBBDraftBoardSortFilterProxyModel.h"
#include "FBB/FBBDraftBoardModel.h"
#include "FBB/FBBPlayerDataService.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"

FBBDraftBoardSortFilterProxyModel::FBBDraftBoardSortFilterProxyModel(FBBPlayer::Projection::TypeMask typeMask, QObject* parent)
    : QSortFilterProxyModel(parent)
    , m_typeMask(typeMask)
{
    connect(&FBBLeaugeSettings::Instance(), &FBBLeaugeSettings::SettingsChanged, this, [=]() {
        invalidateFilter();
    });
}

bool FBBDraftBoardSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    const QVariant& leftData = sourceModel()->data(left, FBBDraftBoardModel::RawDataRole);
    const QVariant& rightData = sourceModel()->data(right, FBBDraftBoardModel::RawDataRole);
    return leftData < rightData;
}

bool FBBDraftBoardSortFilterProxyModel::filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const
{
    switch (sourceColumn)
    {
    case FBBDraftBoardModel::COLUMN_ID:
        return false;
    default:
        break;
    }

    if (m_typeMask & FBBPlayer::Projection::PROJECTION_TYPE_HITTING && sourceColumn >= FBBDraftBoardModel::COLUMN_FIRST_PITCHING && sourceColumn <= FBBDraftBoardModel::COLUMN_LAST_PITCHING) {
        return false;
    }

    if (m_typeMask & FBBPlayer::Projection::PROJECTION_TYPE_PITCHING && sourceColumn >= FBBDraftBoardModel::COLUMN_FIRST_HITTING && sourceColumn <= FBBDraftBoardModel::COLUMN_LAST_HITTING) {
        return false;
    }

    return true;
}

bool FBBDraftBoardSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    const FBBPlayer* pPlayer = FBBPlayerDataService::GetPlayer(sourceRow);

    // Ignore mismatch types
    if (!(pPlayer->spProjection->type & m_typeMask)) {
        return false;
    }

    if (!FBBPlayerDataService::IsValidUnderCurrentSettings(pPlayer)) {
        return false;
    }

    return true;
}
