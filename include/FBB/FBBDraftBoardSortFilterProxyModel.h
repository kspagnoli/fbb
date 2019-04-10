#pragma once

#include "FBB/FBBPlayer.h"

#include <QSortFilterProxyModel>

class FBBDraftBoardSortFilterProxyModel : public QSortFilterProxyModel
{
public:

    enum FilterIn_Bits
    {
        FilterIn_Hitters  = 1 << 0,
        FilterIn_Pitchers = 1 << 1,
        FilterIn_Drafted  = 1 << 2,
        FilterIn_C        = 1 << 3,
        FilterIn_1B       = 1 << 4,
        FilterIn_2B       = 1 << 5,
        FilterIn_SS       = 1 << 6,
        FilterIn_3B       = 1 << 7,
        FilterIn_OF       = 1 << 8,
    };
    using FilterIn = uint32_t;

    FBBDraftBoardSortFilterProxyModel(QObject* parent = nullptr);

    void SetFilter(FilterIn filter);

    virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    virtual bool filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const override;
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:

    FilterIn m_filter = 0;
};