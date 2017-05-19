#include "FBB/FBBDraftBoard.h"
#include "FBB/FBBDraftBoardModel.h"
#include "FBB/FBBDraftBoardSortFilterProxyModel.h"
#include "FBB/FBBPlayerDataService.h"
#include "FBB/FBBDraftDialog.h"

#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QEvent>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QLineEdit>
#include <QCompleter>

FBBDraftBoard::FBBDraftBoard(FBBPlayer::Projection::TypeMask typeMask, QWidget* parent)
    : QWidget(parent)
{
    // Main layout
    QVBoxLayout* pLayout = new QVBoxLayout(this);

    // Source model
    FBBDraftBoardModel* pSourceModel = new FBBDraftBoardModel(this);

    // Proxy model
    FBBDraftBoardSortFilterProxyModel* pProxyModel = new FBBDraftBoardSortFilterProxyModel(typeMask, this);
    pProxyModel->setSourceModel(pSourceModel);

    // Header + layout
    QWidget* pHeader = new QWidget(this);
    QHBoxLayout* pHeaderLayout = new QHBoxLayout(pHeader);
    pHeaderLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(pHeader);

    // Draft button
    QPushButton* pDraftButton = new QPushButton("Draft");
    pDraftButton->setDisabled(true);
    pHeaderLayout->addWidget(pDraftButton);

    // Z/$ button
    QPushButton* pZscoreToggle = new QPushButton("#/z");
    pZscoreToggle->setCheckable(true);
    pHeaderLayout->addWidget(pZscoreToggle);

    // Place holders
    pHeaderLayout->addStretch();

    // Completer
    QCompleter* pSearchCompleter = new QCompleter(this);
    pSearchCompleter->setModel(pProxyModel);
    pSearchCompleter->setCompletionColumn(FBBDraftBoardModel::COLUMN_NAME);
    pSearchCompleter->setCompletionRole(Qt::DisplayRole);
    pSearchCompleter->setFilterMode(Qt::MatchContains);
    pSearchCompleter->setCaseSensitivity(Qt::CaseInsensitive);

    // Search box
    QLineEdit* pLineEdit = new QLineEdit();
    pLineEdit->setCompleter(pSearchCompleter);
    pLineEdit->setClearButtonEnabled(true);
    pHeaderLayout->addWidget(pLineEdit);

    // Table
    QTableView* pTableView = new QTableView(this);
    pTableView->setObjectName("DraftBoardTableView");
    pTableView->setAlternatingRowColors(true);
    pTableView->setSortingEnabled(true);
    pTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    pTableView->verticalHeader()->setDefaultSectionSize(15);
    pTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    pTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    pTableView->setModel(pProxyModel);
    pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    pTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    pTableView->setSortingEnabled(true);
    pTableView->sortByColumn(0, Qt::SortOrder::AscendingOrder);
    pLayout->addWidget(pTableView);

    // Draft button activation
    connect(pTableView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [=](const QModelIndex & current, const QModelIndex & previous) {
        pDraftButton->setEnabled(current.isValid());
    });

    // Draft button interactions
    connect(pDraftButton, &QPushButton::released, this, [=]() {
        const QModelIndex srcIdx = pProxyModel->mapToSource(pTableView->selectionModel()->currentIndex());
        FBBPlayer* pPlayer = FBBPlayerDataService::GetPlayer(srcIdx.row());
        FBBDraftDialog dialog(pPlayer);
        dialog.exec();
    });

    connect(pZscoreToggle, &QPushButton::toggled, this, [=](bool checked) {
        pSourceModel->SetMode(checked ? FBBDraftBoardModel::Mode::Z_SCORE : FBBDraftBoardModel::Mode::STAT);
    });

    // Search activation
    connect(pSearchCompleter, static_cast<void(QCompleter::*)(const QModelIndex&)>(&QCompleter::activated), this, [=](const QModelIndex& index) {
        QAbstractProxyModel* pCompletionModel = reinterpret_cast<QAbstractProxyModel*>(pSearchCompleter->completionModel());
        const QModelIndex proxyIndex = pCompletionModel->mapToSource(index);
        pTableView->selectRow(proxyIndex.row());
        pTableView->setFocus();
    });

    // Table header context menu
    connect(pTableView->horizontalHeader(), &QWidget::customContextMenuRequested, this, [=](const QPoint &pos) {

        QPoint globalPos = pTableView->horizontalHeader()->mapToGlobal(pos);

        QMenu menu;

        menu.setStyleSheet(R"""(
            QMenu::item {
                font-size: 10px;
                padding: 2px 2px 2px 20px;
                border: 1px solid transparent;
            }
            )""");

        for (int i = 0; i < pSourceModel->columnCount(pSourceModel->index(0, 0)); i++) {
            QString columnName = pSourceModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
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
        pTableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
        pTableView->horizontalHeader()->setStretchLastSection(true);
    // });
    
}

