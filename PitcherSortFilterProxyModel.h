#pragma once

#include <QSortFilterProxyModel>

class PitcherSortFilterProxyModel : public QSortFilterProxyModel
{
public:

    PitcherSortFilterProxyModel();

    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

public slots:

    void OnFilterAvailable(bool checked);

private:

    // filters
    bool m_filterActive = false;
};
