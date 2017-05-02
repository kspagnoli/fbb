#pragma once

#include "FBB/FBBPlayer.h"

#include <QWidget>

class QTableView;
class FBBDraftBoardModel;
class FBBDraftBoardSortFilterProxyModel;

class FBBDraftBoard : public QWidget
{
public:
    FBBDraftBoard(FBBPlayer::Projection::Type type, QWidget* parent = nullptr);

private:
    QTableView* m_pTableView;
    FBBDraftBoardModel* m_pSourceModel;
    FBBDraftBoardSortFilterProxyModel* m_pProxyModel;
};