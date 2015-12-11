#pragma once

#include <QSortFilterProxyModel>

class HitterSortFilterProxyModel : public QSortFilterProxyModel
{
public:

    HitterSortFilterProxyModel();

    virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

public slots:

    void OnFilterNL(bool checked);
    void OnFilterAL(bool checked);
    void OnFilterFA(bool checked);

    void OnFilterC(bool checked);
    void OnFilter1B(bool checked);
    void OnFilter2B(bool checked);
    void OnFilterSS(bool checked);
    void OnFilter3B(bool checked);
    void OnFilterOF(bool checked);
    void OnFilterDH(bool checked);
    
private:

    // filters
    bool m_acceptNL = true;
    bool m_acceptAL = true;
    bool m_acceptFA = true;
    bool m_acceptC = true;
    bool m_accept1B = true;
    bool m_accept2B = true;
    bool m_acceptSS = true;
    bool m_accept3B = true;
    bool m_acceptOF = true;
    bool m_acceptDH = true;
};
