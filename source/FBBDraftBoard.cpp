#include "FBB/FBBDraftBoard.h"
#include "FBB/FBBDraftBoardModel.h"
#include "FBB/FBBDraftBoardSortFilterProxyModel.h"

#include <QHBoxLayout>
#include <QTableView>
#include <QHeaderView>

FBBDraftBoard::FBBDraftBoard(QWidget* parent)
    : QWidget(parent)
{
    QHBoxLayout* pLayout = new QHBoxLayout(this);

    // Source 
    m_pSourceModel = new FBBDraftBoardModel(this);

    // Proxy
    m_pProxyModel = new FBBDraftBoardSortFilterProxyModel(this);
    m_pProxyModel->setSourceModel(m_pSourceModel);

    // Table
    m_pTableView = new QTableView(this);
    m_pTableView->setObjectName("draftBoard");
    m_pTableView->setAlternatingRowColors(true);
    m_pTableView->setSortingEnabled(true);
    m_pTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_pTableView->verticalHeader()->setDefaultSectionSize(15);
    m_pTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_pTableView->setModel(m_pProxyModel);
    pLayout->addWidget(m_pTableView);

    // connect(m_pModel, &FBBDraftBoardModel::modelReset, this, [=] {
        m_pTableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
        m_pTableView->horizontalHeader()->setStretchLastSection(true);
    // });
    
}