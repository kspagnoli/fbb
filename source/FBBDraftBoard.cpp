#include "FBB/FBBDraftBoard.h"
#include "FBB/FBBDraftBoardModel.h"
#include "FBB/FBBDraftBoardSortFilterProxyModel.h"

#include <QHBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QEvent>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>

// XXX
FBBDraftBoard::FBBDraftBoard(FBBPlayer::Projection::Type type, QWidget* parent)
    : QWidget(parent)
{
    QHBoxLayout* pLayout = new QHBoxLayout(this);

    // Source 
    m_pSourceModel = new FBBDraftBoardModel(this);

    // Proxy
    m_pProxyModel = new FBBDraftBoardSortFilterProxyModel(type, this);
    m_pProxyModel->setSourceModel(m_pSourceModel);

    // Table
    m_pTableView = new QTableView(this);
    m_pTableView->setObjectName("draftBoard");
    m_pTableView->setAlternatingRowColors(true);
    m_pTableView->setSortingEnabled(true);
    m_pTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_pTableView->verticalHeader()->setDefaultSectionSize(15);
    m_pTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_pTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    m_pTableView->setModel(m_pProxyModel);
    pLayout->addWidget(m_pTableView);

    // Table header context menu
    connect(m_pTableView->horizontalHeader(), &QWidget::customContextMenuRequested, this, [=](const QPoint &pos) {

        QPoint globalPos = m_pTableView->horizontalHeader()->mapToGlobal(pos);

        QMenu menu;

        menu.setStyleSheet(R"""(
            QMenu::item {
                font-size: 10px;
                padding: 2px 2px 2px 20px;
                border: 1px solid transparent;
            }
            )""");

        for (int i = 0; i < m_pSourceModel->columnCount(m_pSourceModel->index(0, 0)); i++) {
            QString columnName = m_pSourceModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
            menu.addAction(columnName);
        }
        // ...

        QAction* selectedItem = menu.exec(globalPos);
        if (selectedItem)
        {
            // something was chosen, do stuff
        }
        else
        {
            // nothing was chosen
        }

    });

    // connect(m_pSourceModel, &FBBDraftBoardModel::modelReset, this, [=] {
        m_pTableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
        m_pTableView->horizontalHeader()->setStretchLastSection(true);
    // });
    
}