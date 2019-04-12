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

void FBBDraftBoardSortFilterProxyModel::SetFilter(FilterIn filter)
{
    m_filter = filter;
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

    if (pPlayer->type == FBBPlayer::PLAYER_TYPE_HITTER && !(m_filter & FilterIn_Hitters)) {
        return false;
    }

    if (pPlayer->type == FBBPlayer::PLAYER_TYPE_PITCHER) {
        if (m_filter & FilterIn_Pitchers) {
            return true;
        } else {
            return false;
        }
    }

    if (m_filter & FilterIn_Drafted && pPlayer->draftInfo.owner != 0) {
        return false;
    }

    const FBBPositionMask posMask = pPlayer->EligablePositions();

    if (posMask & FBB_POSITION_C && (m_filter & FilterIn_C)) {
        return true;
    }

    if (posMask & FBB_POSITION_1B && (m_filter & FilterIn_1B)) {
        return true;
    }
    if (posMask & FBB_POSITION_2B && (m_filter & FilterIn_2B)) {
        return true;
    }
    if (posMask & FBB_POSITION_SS && (m_filter & FilterIn_SS)) {
        return true;
    }
    if (posMask & FBB_POSITION_3B && (m_filter & FilterIn_3B)) {
        return true;
    }
    if (posMask & FBB_POSITION_OF && (m_filter & FilterIn_OF)) {
        return true;
    }

    return false;
}
