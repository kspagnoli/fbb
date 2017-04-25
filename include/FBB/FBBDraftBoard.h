#pragma once

#include <QWidget>

class QTableView;
class FBBDraftBoardModel;
class FBBDraftBoardSortFilterProxyModel;

class FBBDraftBoard : public QWidget
{
public:
    FBBDraftBoard(QWidget* parent = nullptr);

private:
    QTableView* m_pTableView;
    FBBDraftBoardModel* m_pSourceModel;
    FBBDraftBoardSortFilterProxyModel* m_pProxyModel;
};