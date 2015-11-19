#pragma once

#include <QSortFilterProxyModel>

class HitterSortFilterProxyModel : public QSortFilterProxyModel
{
public:

    HitterSortFilterProxyModel();

    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

public slots:

    void OnFilterAvailable(bool checked);

private:

    // filters
    bool m_filterActive = false;
};
