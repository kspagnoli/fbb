#pragma once

#include <QSortFilterProxyModel>

#include "Player.h"

class PlayerSortFilterProxyModel : public QSortFilterProxyModel
{
public:

    PlayerSortFilterProxyModel(Player::CatergoryMask catergory);

    virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    virtual bool filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const override;
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

public slots:

    void OnFilterDrafted(bool checked);
    void OnFilterReplacement(bool checked);

    void OnFilterNL(bool checked);
    void OnFilterAL(bool checked);
    void OnFilterFA(bool checked);

    void OnFilterC(bool checked);
    void OnFilter1B(bool checked);
    void OnFilter2B(bool checked);
    void OnFilterSS(bool checked);
    void OnFilter3B(bool checked);
    void OnFilterOF(bool checked);
    void OnFilterCI(bool checked);
    void OnFilterMI(bool checked);
    void OnFilterDH(bool checked);
    void OnFilterU(bool checked);

    void OnFilterP(bool checked);
    void OnFilterSP(bool checked);
    void OnFilterRP(bool checked);
    
private:

    // Main type
    Player::CatergoryMask m_catergory;

    // General filters
    bool m_acceptDrafted = true;
    bool m_acceptReplacement = true;
    bool m_acceptNL = true;
    bool m_acceptAL = true;
    bool m_acceptFA = true;

    // Hitting filters
    bool m_acceptC = true;
    bool m_accept1B = true;
    bool m_accept2B = true;
    bool m_acceptSS = true;
    bool m_accept3B = true;
    bool m_acceptOF = true;
    bool m_acceptMI = true;
    bool m_acceptCI = true;
    bool m_acceptDH = true;
    bool m_acceptU  = true;

    // Pitching filters
    bool m_acceptP  = true;
    bool m_acceptSP = true;
    bool m_acceptRP = true;
};
