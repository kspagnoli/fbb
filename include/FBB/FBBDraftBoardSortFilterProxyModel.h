#pragma once

#include "FBB/FBBPlayer.h"

#include <QSortFilterProxyModel>

class FBBDraftBoardSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    FBBDraftBoardSortFilterProxyModel(FBBPlayer::Projection::TypeMask typeMask, QObject* parent = nullptr);

    virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    virtual bool filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const override;
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    const FBBPlayer::Projection::TypeMask m_typeMask;
};