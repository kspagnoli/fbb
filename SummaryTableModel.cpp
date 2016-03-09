#include "SummaryTableModel.h"
#include "OwnerSortFilterProxyModel.h"
#include "DraftSettings.h"

SummaryTableModel::SummaryTableModel(const std::vector<OwnerSortFilterProxyModel*>& vecOwnerSortFilterProxyModel, PlayerTableModel* playerTableModel, QWidget* parent)
    : m_vecOwnerSortFilterProxyModels(vecOwnerSortFilterProxyModel)
    , m_playerTableModel(playerTableModel)
    , m_vecOwnerPoints(vecOwnerSortFilterProxyModel.size())
{
}

int SummaryTableModel::rowCount(const QModelIndex& index) const
{
    return int(m_vecOwnerSortFilterProxyModels.size());
}

int SummaryTableModel::columnCount(const QModelIndex& index) const
{
    return RankRows::COUNT;
}

QVariant SummaryTableModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignmentFlag(int(Qt::AlignCenter | Qt::AlignVCenter));
    }

    if (role == Qt::ToolTipRole) {
        switch (index.column())
        {
        case BA:
            return m_playerTableModel->GetTargetValue(PlayerTableModel::COLUMN_AVG);
        case R:
            return m_playerTableModel->GetTargetValue(PlayerTableModel::COLUMN_R);
        case HR:
            return m_playerTableModel->GetTargetValue(PlayerTableModel::COLUMN_HR);
        case RBI:
            return m_playerTableModel->GetTargetValue(PlayerTableModel::COLUMN_RBI);
        case SB:
            return m_playerTableModel->GetTargetValue(PlayerTableModel::COLUMN_SB);
        case ERA:
            return m_playerTableModel->GetTargetValue(PlayerTableModel::COLUMN_ERA);
        case WHIP:
            return m_playerTableModel->GetTargetValue(PlayerTableModel::COLUMN_WHIP);
        case W:
            return m_playerTableModel->GetTargetValue(PlayerTableModel::COLUMN_W);
        case K:
            return m_playerTableModel->GetTargetValue(PlayerTableModel::COLUMN_SO);
        case S:
            return m_playerTableModel->GetTargetValue(PlayerTableModel::COLUMN_SV);
        case SUM:
            return 10 * ((DraftSettings::Get().OwnerCount + 1) * 0.75);
        default:
            break;
        }
    }

    if ((role != Qt::DisplayRole) && (role != RawDataRole) && (role != RankRole)) {
        return QVariant();
    }

    auto ownerSortFilterProxyModel = m_vecOwnerSortFilterProxyModels[index.row()];
    auto ownerPoints = m_vecOwnerPoints[index.row()];

    switch (index.column())
    {
    case RankRows::TEAM:
    {
        if (role == RawDataRole || role == RankRole) {
            return index.row();
        } else {
            return DraftSettings::Get().OwnerAbbreviations[index.row() + 1];
        }
    }
    case RankRows::BUDGET:
    {
        auto budget = ownerSortFilterProxyModel->GetRemainingBudget();
        if (role == RawDataRole || role == RankRole) {
            return budget;
        } else {
            return QString("$%1").arg(budget);
        }
    }
    case RankRows::ROSTER_SIZE:
    {
        auto rosterSize = ownerSortFilterProxyModel->GetRosterSlotsFilled();
        if (role == RawDataRole || role == RankRole) {
            return rosterSize;
        } else {
            return QString("%1").arg(rosterSize);
        }
    }
    case RankRows::BA:
    {
        auto BA = ownerSortFilterProxyModel->AVG();
        if (role == RawDataRole) {
            return BA;
        } else if (role == RankRole) {
            return ownerPoints.rankAVG;
        } else {
            QString strBA = (BA == 0.f ? QString("--") : QString::number(BA, 'f', 3));
            return QString("%1").arg(strBA);
        }
    }
    case RankRows::R:
    {
        auto R = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_R);
        if (role == RawDataRole) {
            return R;
        } else if (role == RankRole) {
            return ownerPoints.rankR;
        } else {
            return QString("%1").arg(R);
        }
    }
    case RankRows::HR:
    {
        auto HR = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_HR);
        if (role == RawDataRole) {
            return HR;
        } else if (role == RankRole) {
            return ownerPoints.rankHR;
        } else {
            return QString("%1").arg(HR);
        }
    }
    case RankRows::RBI:
    {
        auto RBI = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_RBI);
        if (role == RawDataRole) {
            return RBI;
        } else if (role == RankRole) {
            return ownerPoints.rankRBI;
        } else {
            return QString("%1").arg(RBI);
        }
    }
    case RankRows::SB:
    {
        auto SB = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_SB);
        if (role == RawDataRole) {
            return SB;
        } else if (role == RankRole) {
            return ownerPoints.rankSB;
        } else {
            return QString("%1").arg(SB);
        }
    }
    case RankRows::ERA:
    {
        auto ERA = ownerSortFilterProxyModel->ERA();
        if (role == RawDataRole) {
            return ERA;
        } else if (role == RankRole) {
            return ownerPoints.rankERA;
        } else {
            QString strERA = (ERA == 0.f ? QString("--") : QString::number(ERA, 'f', 3));
            return QString("%1").arg(strERA);
        }
    }
    case RankRows::WHIP:
    {
        auto WHIP = ownerSortFilterProxyModel->WHIP();
        if (role == RawDataRole) {
            return WHIP;
        } else if (role == RankRole) {
            return ownerPoints.rankWHIP;
        } else {
            QString strWHIP = (WHIP == 0.f ? QString("--") : QString::number(WHIP, 'f', 3));
            return QString("%1").arg(strWHIP);
        }
    }
    case RankRows::W:
    {
        auto W = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_W);
        if (role == RawDataRole) {
            return W;
        } else if (role == RankRole) {
            return ownerPoints.rankW;
        } else {
            return QString("%1").arg(W);
        }
    }
    case RankRows::K:
    {
        auto SO = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_SO);
        if (role == RawDataRole) {
            return SO;
        } else if (role == RankRole) {
            return ownerPoints.rankK;
        } else {
            return QString("%1").arg(SO);
        }
    }
    case RankRows::S:
    {
        auto SV = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_SV);
        if (role == RawDataRole) {
            return SV;
        } else if (role == RankRole) {
            return ownerPoints.rankSV;
        } else {
            return QString("%1").arg(SV);
        }
    }
    case RankRows::SUM:
    {
        if (role == RawDataRole) {
            return ownerPoints.SUM;
        } else if (role == RankRole) {
            return ownerPoints.rankSUM;
        } else {
            return QString("%1").arg(ownerPoints.SUM);
        }
    }
    }

    return QVariant();
}

QVariant SummaryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // Only care about display roles
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Horizontal) {

        switch (section)
        {
        case RankRows::TEAM:
            return "Team";
        case RankRows::BUDGET:
            return "Budget";
        case RankRows::ROSTER_SIZE:
            return "# Players";
        case RankRows::BA:
            return "AVG";
        case RankRows::R:
            return "R";
        case RankRows::HR:
            return "HR";
        case RankRows::RBI:
            return "RBI";
        case RankRows::SB:
            return "SB";
        case RankRows::ERA:
            return "ERA";
        case RankRows::WHIP:
            return "WHIP";
        case RankRows::W:
            return "W";
        case RankRows::K:
            return "K";
        case RankRows::S:
            return "S";
        case RankRows::SUM:
            return "SUM";
        default:
            break;
        }
    }

    return QVariant();
}

void SummaryTableModel::OnDraftedEnd()
{
    // All the data is changing
    emit beginResetModel();

    auto RankValues = [=](auto FnGet, auto FnSet)
    {
        std::vector<uint32_t> ranks(DraftSettings::Get().OwnerCount);
        std::vector<float> values(DraftSettings::Get().OwnerCount);
        std::size_t n(0);
        std::generate(std::begin(ranks), std::end(ranks), [&] { return n++; });

        for (size_t i = 0; i < m_vecOwnerSortFilterProxyModels.size(); i++) {
            values[i] = FnGet(i);
        }

        std::sort(std::begin(ranks), std::end(ranks), [&](uint32_t iLHS, uint32_t iRHS) {
            return values[iLHS] < values[iRHS];
        });

        for (size_t i = 0; i < m_vecOwnerPoints.size(); i++) {
            FnSet(ranks[i], i + 1);
        }
    };

    // AVG
    RankValues(
        [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->AVG(); },
        [=](size_t i, float x) { return m_vecOwnerPoints[i].rankAVG = x; }
    );

    // R
    RankValues(
        [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_R); },
        [=](size_t i, float x) { return m_vecOwnerPoints[i].rankR = x; }
    );

    // HR
    RankValues(
        [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_HR); },
        [=](size_t i, float x) { return m_vecOwnerPoints[i].rankHR = x; }
    );

    // RBI
    RankValues(
        [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_RBI); },
        [=](size_t i, float x) { return m_vecOwnerPoints[i].rankRBI = x; }
    );

    // SB
    RankValues(
        [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_SB); },
        [=](size_t i, float x) { return m_vecOwnerPoints[i].rankSB = x; }
    );

    // ERA
    RankValues(
        [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->ERA(); },
        [=](size_t i, float x) { return m_vecOwnerPoints[i].rankERA = x; }
    );

    // WHIP
    RankValues(
        [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->WHIP(); },
        [=](size_t i, float x) { return m_vecOwnerPoints[i].rankWHIP = x; }
    );

    // W
    RankValues(
        [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_W); },
        [=](size_t i, float x) { return m_vecOwnerPoints[i].rankW = x; }
    );

    // K
    RankValues(
        [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_SO); },
        [=](size_t i, float x) { return m_vecOwnerPoints[i].rankK = x; }
    );

    // SV
    RankValues(
        [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_SV); },
        [=](size_t i, float x) { return m_vecOwnerPoints[i].rankSV = x; }
    );

    // Invert "lower-is-better" stats
    for (auto& owner : m_vecOwnerPoints) {
        owner.rankERA = m_vecOwnerPoints.size() - owner.rankERA + 1;
        owner.rankWHIP = m_vecOwnerPoints.size() - owner.rankWHIP + 1;
    }

    // Calculate sum
    for (auto& owner : m_vecOwnerPoints) {
        owner.SUM = owner.rankAVG + owner.rankR + owner.rankHR + owner.rankRBI + owner.rankSB + owner.rankERA + owner.rankWHIP + owner.rankW + owner.rankK + owner.rankSV;
    }

    // SV
    RankValues(
        [=](size_t i) { return m_vecOwnerPoints[i].SUM; },
        [=](size_t i, float x) { return m_vecOwnerPoints[i].rankSUM = x; }
    );

    // All the data is changing
    emit endResetModel();
}
