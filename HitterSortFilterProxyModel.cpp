#include "HitterSortFilterProxyModel.h"
#include "HitterTableModel.h"
#include "Teams.h"
#include "Hitter.h"

//------------------------------------------------------------------------------
// HitterSortFilterProxyModel
//------------------------------------------------------------------------------
HitterSortFilterProxyModel::HitterSortFilterProxyModel() = default;

//------------------------------------------------------------------------------
// lessThan (override)
//------------------------------------------------------------------------------
bool HitterSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    const QVariant& leftData = sourceModel()->data(left, HitterTableModel::RawDataRole);
    const QVariant& rightData = sourceModel()->data(right, HitterTableModel::RawDataRole);

    return leftData < rightData;
}

//------------------------------------------------------------------------------
// filterAcceptsRow (override)
//------------------------------------------------------------------------------
bool HitterSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    const QModelIndex& teamIndex = sourceModel()->index(sourceRow, HitterTableModel::COLUMN_TEAM, sourceParent);
    const QString& team = sourceModel()->data(teamIndex).toString();

    const QModelIndex& posIndex = sourceModel()->index(sourceRow, HitterTableModel::COLUMN_POSITION, sourceParent);
    const uint32_t& pos = sourceModel()->data(posIndex, HitterTableModel::RawDataRole).toInt();

    auto AcceptTeam = [&]() -> bool
    {
        if (m_acceptNL && LookupTeamGroup(team.toStdString()).leauge == Leauge::NL) {
            return true;
        }

        if (m_acceptAL && LookupTeamGroup(team.toStdString()).leauge == Leauge::AL) {
            return true;
        }

        if (m_acceptFA && LookupTeamGroup(team.toStdString()).leauge == Leauge::Unknown) {
            return true;
        }

        return false;
    };

    auto AcceptPosition = [&]() -> bool
    {
        // HAX
        if (pos == uint32_t(Hitter::Position::None)) {
            return true;
        }

        if (m_acceptC && (pos & uint32_t(Hitter::Position::Catcher))) {
            return true;
        }

        if (m_accept1B && (pos & uint32_t(Hitter::Position::First))) {
            return true;
        }

        if (m_accept2B && (pos & uint32_t(Hitter::Position::Second))) {
            return true;
        }

        if (m_acceptSS && (pos & uint32_t(Hitter::Position::SS))) {
            return true;
        }

        if (m_accept3B && (pos & uint32_t(Hitter::Position::Third))) {
            return true;
        }

        if (m_acceptOF && (pos & uint32_t(Hitter::Position::Outfield))) {
            return true;
        }

        if (m_acceptDH && (pos & uint32_t(Hitter::Position::DH))) {
            return true;
        }

        return false;
    };

    return AcceptTeam() && AcceptPosition();
}

//------------------------------------------------------------------------------
// OnFilterNL
//------------------------------------------------------------------------------
void HitterSortFilterProxyModel::OnFilterNL(bool checked)
{
    m_acceptNL = checked;
    QSortFilterProxyModel::filterChanged();
}

//------------------------------------------------------------------------------
// OnFilterAL
//------------------------------------------------------------------------------
void HitterSortFilterProxyModel::OnFilterAL(bool checked)
{
    m_acceptAL = checked;
    QSortFilterProxyModel::filterChanged();
}

//------------------------------------------------------------------------------
// OnFilterFA
//------------------------------------------------------------------------------
void HitterSortFilterProxyModel::OnFilterFA(bool checked)
{
    m_acceptFA = checked;
    QSortFilterProxyModel::filterChanged();
}

//------------------------------------------------------------------------------
// OnFilterC
//------------------------------------------------------------------------------
void HitterSortFilterProxyModel::OnFilterC(bool checked)
{
    m_acceptC = checked;
    QSortFilterProxyModel::filterChanged();
}

//------------------------------------------------------------------------------
// OnFilter1B
//------------------------------------------------------------------------------
void HitterSortFilterProxyModel::OnFilter1B(bool checked)
{
    m_accept1B = checked;
    QSortFilterProxyModel::filterChanged();
}

//------------------------------------------------------------------------------
// OnFilter2B
//------------------------------------------------------------------------------
void HitterSortFilterProxyModel::OnFilter2B(bool checked)
{
    m_accept2B = checked;
    QSortFilterProxyModel::filterChanged();
}

//------------------------------------------------------------------------------
// OnFilterSS
//------------------------------------------------------------------------------
void HitterSortFilterProxyModel::OnFilterSS(bool checked)
{
    m_acceptSS = checked;
    QSortFilterProxyModel::filterChanged();
}

//------------------------------------------------------------------------------
// OnFilter3B
//------------------------------------------------------------------------------
void HitterSortFilterProxyModel::OnFilter3B(bool checked)
{
    m_accept3B = checked;
    QSortFilterProxyModel::filterChanged();
}

//------------------------------------------------------------------------------
// OnFilterOF
//------------------------------------------------------------------------------
void HitterSortFilterProxyModel::OnFilterOF(bool checked)
{
    m_acceptOF = checked;
    QSortFilterProxyModel::filterChanged();
}

//------------------------------------------------------------------------------
// OnFilterDH
//------------------------------------------------------------------------------
void HitterSortFilterProxyModel::OnFilterDH(bool checked)
{
    m_acceptDH = checked;
    QSortFilterProxyModel::filterChanged();
}


