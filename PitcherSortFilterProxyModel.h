#pragma once

#include <QSortFilterProxyModel>

class PitcherSortFilterProxyModel : public QSortFilterProxyModel
{
public:

    PitcherSortFilterProxyModel();

    virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

public slots:

    void OnFilterNL(bool checked);
    void OnFilterAL(bool checked);
    void OnFilterFA(bool checked);

    void OnFilterSP(bool checked);
    void OnFilterRP(bool checked);

private:

    // filters
    bool m_acceptNL = true;
    bool m_acceptAL = true;
    bool m_acceptFA = true;
    bool m_acceptSP = true;
    bool m_acceptRP = true;
};
