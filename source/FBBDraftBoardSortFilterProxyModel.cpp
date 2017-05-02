#include "FBB/FBBDraftBoardSortFilterProxyModel.h"
#include "FBB/FBBDraftBoardModel.h"
#include "FBB/FBBPlayerDataService.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"

FBBDraftBoardSortFilterProxyModel::FBBDraftBoardSortFilterProxyModel(FBBPlayer::Projection::Type type, QObject* parent)
    : QSortFilterProxyModel(parent)
    , m_type(type)
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
    if (m_type == FBBPlayer::Projection::PROJECTION_TYPE_HITTING && sourceColumn >= FBBDraftBoardModel::COLUMN_FIRST_PITCHING && sourceColumn <= FBBDraftBoardModel::COLUMN_LAST_PITCHING) {
        return false;
    }

    if (m_type == FBBPlayer::Projection::PROJECTION_TYPE_PITCHING && sourceColumn >= FBBDraftBoardModel::COLUMN_FIRST_HITTING && sourceColumn <= FBBDraftBoardModel::COLUMN_FIRST_PITCHING) {
        return false;
    }

    return true;
}

bool FBBDraftBoardSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    const FBBPlayer* pPlayer = FBBPlayerDataService::GetPlayer(sourceRow);

    // Ignore mismatch types
    if (pPlayer->spProjection->type != m_type) {
        return false;
    }

    // Ignore FA
    if (!FBBLeaugeSettings::Instance().projections.includeFA && pPlayer->team == FBBTeam::FA) {
        return false;
    }

    // Ignore min AB
    if (pPlayer->spProjection->type == FBBPlayer::Projection::PROJECTION_TYPE_HITTING && pPlayer->spProjection->hitting.AB <= FBBLeaugeSettings::Instance().projections.minAB) {
        return false;
    }

    // Ignore min IP
    if (pPlayer->spProjection->type == FBBPlayer::Projection::PROJECTION_TYPE_PITCHING && pPlayer->spProjection->pitching.IP <= FBBLeaugeSettings::Instance().projections.minIP) {
        return false;
    }

    return true;
}
