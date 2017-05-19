#include "FBB/FBBDraftLog.h"
#include "FBB/FBBDraftLogModel.h"

#include <QHBoxLayout>
#include <QTableView>
#include <QHeaderView>

FBBDraftLog::FBBDraftLog(QWidget* pParent)
    : QWidget(pParent)
{
    // Main layout
    QHBoxLayout* pLayout = new QHBoxLayout(this);

    // Add table view
    QTableView* pTableView = new QTableView(this);
    pTableView->setModel(&FBBDraftLogModel::Instance());
    pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    pTableView->setSelectionMode(QAbstractItemView::NoSelection);
    pTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    pTableView->verticalHeader()->setDefaultSectionSize(15);
    pTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    pTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    pLayout->addWidget(pTableView);
}