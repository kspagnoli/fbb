#include "FBB/FBBDraftLog.h"
#include "FBB/FBBDraftLogModel.h"

#include <QHBoxLayout>
#include <QTreeView>

FBBDraftLog::FBBDraftLog(QWidget* pParent)
    : QWidget(pParent)
{
    // Main layout
    QHBoxLayout* pLayout = new QHBoxLayout(this);

    // Add table view
    QTreeView* pTreeView = new QTreeView(this);
    pTreeView->setModel(&FBBDraftLogModel::Instance());
    pTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    pTreeView->setSelectionMode(QAbstractItemView::NoSelection);
    pTreeView->setUniformRowHeights(true);
    pLayout->addWidget(pTreeView);
}