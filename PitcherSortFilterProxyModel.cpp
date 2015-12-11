#include "PitcherSortFilterProxyModel.h"
#include "PitcherTableModel.h"
#include "Teams.h"

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
    const QModelIndex& teamIndex = sourceModel()->index(sourceRow, PitcherTableModel::COLUMN_TEAM, sourceParent);
    const QString& team = sourceModel()->data(teamIndex).toString();

    const QModelIndex& posIndex = sourceModel()->index(sourceRow, PitcherTableModel::COLUMN_POSITION, sourceParent);
    const QString& pos = sourceModel()->data(posIndex).toString();
    
    if (!m_acceptNL && LookupTeamGroup(team.toStdString()).leauge == Leauge::NL) {
        return false;
    }

    if (!m_acceptAL && LookupTeamGroup(team.toStdString()).leauge == Leauge::AL) {
        return false;
    }

    if (!m_acceptFA && LookupTeamGroup(team.toStdString()).leauge == Leauge::Unknown) {
        return false;
    }

    if (!m_acceptSP && pos.contains("SP")) {
        return false;
    }

    if (!m_acceptRP && pos.contains("RP")) {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
// OnFilterNL
//------------------------------------------------------------------------------
void PitcherSortFilterProxyModel::OnFilterNL(bool checked)
{
    m_acceptNL = checked;
    QSortFilterProxyModel::filterChanged();
}

//------------------------------------------------------------------------------
// OnFilterAL
//------------------------------------------------------------------------------
void PitcherSortFilterProxyModel::OnFilterAL(bool checked)
{
    m_acceptAL = checked;
    QSortFilterProxyModel::filterChanged();
}

//------------------------------------------------------------------------------
// OnFilterFA
//------------------------------------------------------------------------------
void PitcherSortFilterProxyModel::OnFilterFA(bool checked)
{
    m_acceptFA = checked;
    QSortFilterProxyModel::filterChanged();
}

//------------------------------------------------------------------------------
// OnFilterSP
//------------------------------------------------------------------------------
void PitcherSortFilterProxyModel::OnFilterSP(bool checked)
{
    m_acceptSP = checked;
    QSortFilterProxyModel::filterChanged();
}

//------------------------------------------------------------------------------
// OnFilterRP
//------------------------------------------------------------------------------
void PitcherSortFilterProxyModel::OnFilterRP(bool checked)
{
    m_acceptRP = checked;
    QSortFilterProxyModel::filterChanged();
}

