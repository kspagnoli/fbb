#pragma once

#include "FBB/FBBPlayer.h"

#include <QSortFilterProxyModel>

class FBBDraftBoardSortFilterProxyModel : public QSortFilterProxyModel
{
public:

    FBBDraftBoardSortFilterProxyModel(QObject* parent = nullptr);

    void SetOnlyHitters(bool enable);
    void SetOnlyPitchers(bool enable);
    void SetShowDrafted(bool enable);
    void SetPositionFilter(FBBPositionMask mask);

    virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    virtual bool filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const override;
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:

    bool m_onlyHitters = false;
    bool m_onlyPitchers = false;
    bool m_showDrafted = false;
    FBBPositionMask m_positionFilter = 0;
};