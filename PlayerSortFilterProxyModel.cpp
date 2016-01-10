#include "PlayerSortFilterProxyModel.h"
#include "PlayerTableModel.h"
#include "Teams.h"

//------------------------------------------------------------------------------
// PlayerSortFilterProxyModel
//------------------------------------------------------------------------------
PlayerSortFilterProxyModel::PlayerSortFilterProxyModel(Player::CatergoryMask catergory)
    : m_catergory(catergory)
{
}

//------------------------------------------------------------------------------
// lessThan (override)
//------------------------------------------------------------------------------
bool PlayerSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    const QVariant& leftData = sourceModel()->data(left, PlayerTableModel::RawDataRole);
    const QVariant& rightData = sourceModel()->data(right, PlayerTableModel::RawDataRole);

    return leftData < rightData;
}

//------------------------------------------------------------------------------
// filterAcceptColumn (override)
//------------------------------------------------------------------------------
bool PlayerSortFilterProxyModel::filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const
{
    switch (sourceColumn)
    {
    // Both
    case PlayerTableModel::COLUMN_RANK:
    case PlayerTableModel::COLUMN_DRAFT_BUTTON:
    case PlayerTableModel::COLUMN_OWNER:
    case PlayerTableModel::COLUMN_PAID:
    case PlayerTableModel::COLUMN_NAME:
    case PlayerTableModel::COLUMN_TEAM:
    case PlayerTableModel::COLUMN_POSITION:
    case PlayerTableModel::COLUMN_Z:
    case PlayerTableModel::COLUMN_ESTIMATE:
    case PlayerTableModel::COLUMN_COMMENT:
        return true;

    // Hitting
    case PlayerTableModel::COLUMN_AB:
    case PlayerTableModel::COLUMN_AVG:
    case PlayerTableModel::COLUMN_HR:
    case PlayerTableModel::COLUMN_R:
    case PlayerTableModel::COLUMN_RBI:
    case PlayerTableModel::COLUMN_SB:
        return m_catergory == Player::Catergory::Hitter;

    // Pitching
    case PlayerTableModel::COLUMN_IP:
    case PlayerTableModel::COLUMN_SO:
    case PlayerTableModel::COLUMN_ERA:
    case PlayerTableModel::COLUMN_WHIP:
    case PlayerTableModel::COLUMN_W:
    case PlayerTableModel::COLUMN_SV:
        return m_catergory == Player::Catergory::Pitcher;

    default:
        return false;
    }
}

//------------------------------------------------------------------------------
// filterAcceptsRow (override)
//------------------------------------------------------------------------------
bool PlayerSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    auto AcceptCatergoty = [&]() -> bool
    {
        const QModelIndex& catergoryIndex = sourceModel()->index(sourceRow, PlayerTableModel::COLUMN_CATERGORY, sourceParent);
        Player::Catergory catergory = Player::Catergory(sourceModel()->data(catergoryIndex).toUInt());
        return catergory == m_catergory;
    };

    auto AcceptTeam = [&]() -> bool
    {
        const QModelIndex& teamIndex = sourceModel()->index(sourceRow, PlayerTableModel::COLUMN_TEAM, sourceParent);
        const QString& team = sourceModel()->data(teamIndex).toString();

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
        const QModelIndex& posIndex = sourceModel()->index(sourceRow, PlayerTableModel::COLUMN_POSITION, sourceParent);
        const uint32_t& pos = sourceModel()->data(posIndex, PlayerTableModel::RawDataRole).toInt();

        if (pos == uint32_t(PlayerPosition::None)) {
            return true;
        }

        if (m_acceptC && (pos & (1 << uint32_t(PlayerPosition::Catcher)))) {
            return true;
        }

        if (m_accept1B && (pos & (1 << uint32_t(PlayerPosition::First)))) {
            return true;
        }

        if (m_accept2B && (pos & (1 << uint32_t(PlayerPosition::Second)))) {
            return true;
        }

        if (m_acceptSS && (pos & (1 << uint32_t(PlayerPosition::SS)))) {
            return true;
        }

        if (m_accept3B && (pos & (1 << uint32_t(PlayerPosition::Third)))) {
            return true;
        }

        if (m_acceptOF && (pos & (1 << uint32_t(PlayerPosition::Outfield)))) {
            return true;
        }

        if (m_acceptDH && (pos & (1 << uint32_t(PlayerPosition::DH)))) {
            return true;
        }

        if (m_acceptSP && (pos & (1 << uint32_t(PlayerPosition::Starter)))) {
            return true;
        }

        if (m_acceptRP && (pos & (1 << uint32_t(PlayerPosition::Relief)))) {
            return true;
        }

        return false;
    };

    return AcceptCatergoty() && AcceptTeam() && AcceptPosition();
}

//------------------------------------------------------------------------------
// OnFilterNL
//------------------------------------------------------------------------------
void PlayerSortFilterProxyModel::OnFilterNL(bool checked)
{
    m_acceptNL = checked;
    QSortFilterProxyModel::invalidate();
}

//------------------------------------------------------------------------------
// OnFilterAL
//------------------------------------------------------------------------------
void PlayerSortFilterProxyModel::OnFilterAL(bool checked)
{
    m_acceptAL = checked;
    QSortFilterProxyModel::invalidate();
}

//------------------------------------------------------------------------------
// OnFilterFA
//------------------------------------------------------------------------------
void PlayerSortFilterProxyModel::OnFilterFA(bool checked)
{
    m_acceptFA = checked;
    QSortFilterProxyModel::invalidate();
}

//------------------------------------------------------------------------------
// OnFilterC
//------------------------------------------------------------------------------
void PlayerSortFilterProxyModel::OnFilterC(bool checked)
{
    m_acceptC = checked;
    QSortFilterProxyModel::invalidate();
}

//------------------------------------------------------------------------------
// OnFilter1B
//------------------------------------------------------------------------------
void PlayerSortFilterProxyModel::OnFilter1B(bool checked)
{
    m_accept1B = checked;
    QSortFilterProxyModel::invalidate();
}

//------------------------------------------------------------------------------
// OnFilter2B
//------------------------------------------------------------------------------
void PlayerSortFilterProxyModel::OnFilter2B(bool checked)
{
    m_accept2B = checked;
    QSortFilterProxyModel::invalidate();
}

//------------------------------------------------------------------------------
// OnFilterSS
//------------------------------------------------------------------------------
void PlayerSortFilterProxyModel::OnFilterSS(bool checked)
{
    m_acceptSS = checked;
    QSortFilterProxyModel::invalidate();
}

//------------------------------------------------------------------------------
// OnFilter3B
//------------------------------------------------------------------------------
void PlayerSortFilterProxyModel::OnFilter3B(bool checked)
{
    m_accept3B = checked;
    QSortFilterProxyModel::invalidate();
}

//------------------------------------------------------------------------------
// OnFilterOF
//------------------------------------------------------------------------------
void PlayerSortFilterProxyModel::OnFilterOF(bool checked)
{
    m_acceptOF = checked;
    QSortFilterProxyModel::invalidate();
}

//------------------------------------------------------------------------------
// OnFilterDH
//------------------------------------------------------------------------------
void PlayerSortFilterProxyModel::OnFilterDH(bool checked)
{
    m_acceptDH = checked;
    QSortFilterProxyModel::invalidate();
}

//------------------------------------------------------------------------------
// OnFilterSP
//------------------------------------------------------------------------------
void PlayerSortFilterProxyModel::OnFilterSP(bool checked)
{
    m_acceptSP = checked;
    QSortFilterProxyModel::invalidate();
}

//------------------------------------------------------------------------------
// OnFilterRP
//------------------------------------------------------------------------------
void PlayerSortFilterProxyModel::OnFilterRP(bool checked)
{
    m_acceptRP = checked;
    QSortFilterProxyModel::invalidate();
}


