#include "FBB/FBBDraftBoard.h"
#include "FBB/FBBDraftBoardModel.h"
#include "FBB/FBBDraftBoardSortFilterProxyModel.h"
#include "FBB/FBBDraftDialog.h"
#include "FBB/FBBApplication.h"

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
#include <QFontMetrics>
#include <QFrame>
#include <QCheckBox>

class VDivider : public QFrame
{
public:
    VDivider(QWidget* pParent = nullptr)
        : QFrame(pParent)
    {
        setFrameShape(QFrame::VLine);
        setFrameShadow(QFrame::Sunken);
    }
};

class HDivider : public QFrame
{
public:
    HDivider(QWidget* pParent = nullptr)
        : QFrame(pParent)
    {
        setFrameShape(QFrame::HLine);
        setFrameShadow(QFrame::Sunken);
    }
};

FBBDraftBoard::FBBDraftBoard(QWidget* parent)
    : QWidget(parent)
{
    // TODO: Make global...
    QFont font = QFont("Consolas", 9);
    QFontMetrics fm(font);

    // Main layout
    QVBoxLayout* pLayout = new QVBoxLayout(this);

    // Proxy model
    FBBDraftBoardSortFilterProxyModel* pProxyModel = new FBBDraftBoardSortFilterProxyModel(this);
    pProxyModel->setSourceModel(fbbApp->DraftBoardModel());

    // Header + layout
    QWidget* pHeader = new QWidget(this);
    QHBoxLayout* pHeaderLayout = new QHBoxLayout(pHeader);
    pHeaderLayout->setContentsMargins(0, 0, 0, 0);
    pLayout->addWidget(pHeader);

    // Draft button
    QPushButton* pDraftButton = new QPushButton("Draft");
    pDraftButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    pDraftButton->setDisabled(true);
    pHeaderLayout->addWidget(pDraftButton);

    // Divider
    pHeaderLayout->addWidget(new VDivider(this));

    // Stack layout
    QVBoxLayout* pFilterStackLayout = new QVBoxLayout();
    pFilterStackLayout->setContentsMargins(0, 0, 0, 0);
    pHeaderLayout->addLayout(pFilterStackLayout);

    // Bulk filter layout
    QHBoxLayout* pBulkFilterLayout = new QHBoxLayout();
    pBulkFilterLayout->setContentsMargins(0, 0, 0, 0);
    pFilterStackLayout->addLayout(pBulkFilterLayout);

    // Filter hitters button
    QCheckBox* pFilter_Hitters = new QCheckBox("Hitters", this);
    pFilter_Hitters->setCheckable(true);
    pFilter_Hitters->setChecked(true);
    pBulkFilterLayout->addWidget(pFilter_Hitters);

    // Filter pitchers button
    QCheckBox* pFilter_Pitchers = new QCheckBox("Pitchers", this);
    pFilter_Pitchers->setCheckable(true);
    pFilter_Pitchers->setChecked(true);
    pBulkFilterLayout->addWidget(pFilter_Pitchers);

    // Filter drafter button
    QCheckBox* pFilter_Drafted = new QCheckBox("Drafted", this);
    pFilter_Drafted->setCheckable(true);
    pFilter_Drafted->setChecked(true);
    pBulkFilterLayout->addWidget(pFilter_Drafted);

    // Position filter layout
    QHBoxLayout* pPositionFilterLayout = new QHBoxLayout();
    pPositionFilterLayout->setContentsMargins(0, 0, 0, 0);
    pFilterStackLayout->addLayout(pPositionFilterLayout);

    // Filter C button
    QCheckBox* pFilter_C  = new QCheckBox("C", this);
    pFilter_C->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    pFilter_C->setCheckable(true);
    pFilter_C->setChecked(true);
    pPositionFilterLayout->addWidget(pFilter_C);

    // Filter 1B button
    QCheckBox* pFilter_1B  = new QCheckBox("1B", this);
    pFilter_1B->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    pFilter_1B->setCheckable(true);
    pFilter_1B->setChecked(true);
    pPositionFilterLayout->addWidget(pFilter_1B);

    // Filter 2B button
    QCheckBox* pFilter_2B  = new QCheckBox("2B", this);
    pFilter_2B->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    pFilter_2B->setCheckable(true);
    pFilter_2B->setChecked(true);
    pPositionFilterLayout->addWidget(pFilter_2B);

    // Filter SS button
    QCheckBox* pFilter_SS  = new QCheckBox("SS", this);
    pFilter_SS->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    pFilter_SS->setCheckable(true);
    pFilter_SS->setChecked(true);
    pPositionFilterLayout->addWidget(pFilter_SS);

    // Filter 3B button
    QCheckBox* pFilter_3B  = new QCheckBox("3B", this);
    pFilter_3B->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    pFilter_3B->setCheckable(true);
    pFilter_3B->setChecked(true);
    pPositionFilterLayout->addWidget(pFilter_3B);

    // Filter OF button
    QCheckBox* pFilter_OF  = new QCheckBox("OF", this);
    pFilter_OF->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    pFilter_OF->setCheckable(true);
    pFilter_OF->setChecked(true);
    pPositionFilterLayout->addWidget(pFilter_OF);

    // Divider
    pHeaderLayout->addWidget(new VDivider(this));

    QGridLayout* pGridlayout = new QGridLayout();
    pGridlayout->setContentsMargins(0, 0, 0, 0);
    pHeaderLayout->addLayout(pGridlayout);

    // Z/$ button
    QPushButton* pZscoreToggle = new QPushButton("#/z");
    pZscoreToggle->setCheckable(true);
    pGridlayout->addWidget(pZscoreToggle, 0, 0);

    // Place holders
    pHeaderLayout->addStretch();

    // Divider
    pLayout->addWidget(new HDivider(this));

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
    pLineEdit->addAction(QIcon(":/icons/search.png"), QLineEdit::LeadingPosition);
    pHeaderLayout->addWidget(pLineEdit);

    // Table
    QTableView* pTableView = new QTableView(this);
    pTableView->setAlternatingRowColors(true);
    pTableView->setSortingEnabled(true);
    pTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    pTableView->verticalHeader()->hide();
    pTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    pTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    pTableView->setModel(pProxyModel);
    pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    pTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    pTableView->setSortingEnabled(true);
    pTableView->setTextElideMode(Qt::ElideRight);
    pTableView->sortByColumn(FBBDraftBoardModel::COLUMN_RANK, Qt::SortOrder::AscendingOrder);

    pTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_ID,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_RANK,       QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_OWNER,      QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_PAID,       QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_NAME,       QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_TEAM,       QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_AGE,        QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_EXPERIENCE, QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_POSITION,   QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_AB,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_H,          QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_AVG,        QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_HR,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_R,          QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_RBI,        QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_SB,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_IP,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_HA,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_BB,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_ER,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_SO,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_ERA,        QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_WHIP,       QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_W,          QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_SV,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_Z,          QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_ESTIMATE,   QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_COMMENT,    QHeaderView::Stretch);

    const int charWidth = fm.averageCharWidth();
    const int padding = pTableView->style()->pixelMetric(QStyle::PM_HeaderMargin);
    pTableView->verticalHeader()->setDefaultSectionSize(fm.height());
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_RANK,       charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_OWNER,      charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_PAID,       charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_NAME,       charWidth * 25 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_TEAM,       charWidth * 6 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_AGE,        charWidth * 6 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_EXPERIENCE, charWidth * 6 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_POSITION,   charWidth * 13 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_AB,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_H,          charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_AVG,        charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_HR,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_R,          charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_RBI,        charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_SB,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_IP,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_HA,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_BB,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_ER,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_SO,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_ERA,        charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_WHIP,       charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_W,          charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_SV,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_Z,          charWidth * 10 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_ESTIMATE,   charWidth * 10 + padding);

    pTableView->hideColumn(FBBDraftBoardModel::COLUMN_ID);

    pLayout->addWidget(pTableView, 1);

    // Draft button activation
    connect(pTableView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [=](const QModelIndex& current, const QModelIndex& previous) {
        pDraftButton->setEnabled(current.isValid());
        if (current.isValid()) {
            const int row = pProxyModel->mapToSource(current).row();
            QModelIndex idx = fbbApp->DraftBoardModel()->index(row, FBBDraftBoardModel::COLUMN_NAME);
            QString name = fbbApp->DraftBoardModel()->data(idx, Qt::DisplayRole).toString();
            pDraftButton->setText(QString("Draft\n%1").arg(name));
        } else {
            pDraftButton->setText("Draft...");
        }
    });

    // Draft button interactions
    connect(pDraftButton, &QPushButton::released, this, [=]() {
        const QModelIndex srcIdx = pProxyModel->mapToSource(pTableView->selectionModel()->currentIndex());
        FBBPlayer* pPlayer = fbbApp->DraftBoardModel()->GetPlayer(srcIdx.row());
        FBBDraftDialog dialog(pPlayer);
        dialog.exec();
    });

    connect(pZscoreToggle, &QPushButton::toggled, this, [=](bool checked) {
        fbbApp->DraftBoardModel()->SetMode(checked ? FBBDraftBoardModel::Mode::Z_SCORE : FBBDraftBoardModel::Mode::STAT);
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

        for (int i = 0; i < fbbApp->DraftBoardModel()->columnCount(fbbApp->DraftBoardModel()->index(0, 0)); i++) {
            QString columnName = fbbApp->DraftBoardModel()->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
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

    //
    auto OnFilterChanged = [=]()
    {
        bool showHitters = pFilter_Hitters->isChecked();
        for (int i = FBBDraftBoardModel::COLUMN_FIRST_HITTING; i <= FBBDraftBoardModel::COLUMN_LAST_HITTING; i++) {
            pTableView->setColumnHidden(i, !showHitters);
        }

        bool showPitchers = pFilter_Pitchers->isChecked();
        for (int i = FBBDraftBoardModel::COLUMN_FIRST_PITCHING; i <= FBBDraftBoardModel::COLUMN_LAST_PITCHING; i++) {
            pTableView->setColumnHidden(i, !showPitchers);
        }

        FBBDraftBoardSortFilterProxyModel::FilterIn filter = 0;
        if (pFilter_Drafted->isChecked()) {
            filter |= FBBDraftBoardSortFilterProxyModel::FilterIn_Drafted;
        }
        if (pFilter_Hitters->isChecked()) {
            filter |= FBBDraftBoardSortFilterProxyModel::FilterIn_Hitters;
        }
        if (pFilter_Pitchers->isChecked()) {
            filter |= FBBDraftBoardSortFilterProxyModel::FilterIn_Pitchers;
        }
        if (pFilter_Drafted->isChecked()) {
            filter |= FBBDraftBoardSortFilterProxyModel::FilterIn_Drafted;
        }
        if (pFilter_C->isChecked()) {
            filter |= FBBDraftBoardSortFilterProxyModel::FilterIn_C;
        }
        if (pFilter_1B->isChecked()) {
            filter |= FBBDraftBoardSortFilterProxyModel::FilterIn_1B;
        }
        if (pFilter_2B->isChecked()) {
            filter |= FBBDraftBoardSortFilterProxyModel::FilterIn_2B;
        }
        if (pFilter_SS->isChecked()) {
            filter |= FBBDraftBoardSortFilterProxyModel::FilterIn_SS;
        }
        if (pFilter_3B->isChecked()) {
            filter |= FBBDraftBoardSortFilterProxyModel::FilterIn_3B;
        }
        if (pFilter_OF->isChecked()) {
            filter |= FBBDraftBoardSortFilterProxyModel::FilterIn_OF;
        }
        pProxyModel->SetFilter(filter);
    };

    connect(pFilter_Hitters, &QPushButton::released, this, [=]() {
        OnFilterChanged();
    });
    connect(pFilter_Pitchers, &QPushButton::released, this, [=]() {
        OnFilterChanged();
    });
    connect(pFilter_Drafted, &QPushButton::released, this, [=]() {
        OnFilterChanged();
    });
    connect(pFilter_C, &QPushButton::released, this, [=]() {
        OnFilterChanged();
    });
    connect(pFilter_1B, &QPushButton::released, this, [=]() {
        OnFilterChanged();
    });
    connect(pFilter_2B, &QPushButton::released, this, [=]() {
        OnFilterChanged();
    });
    connect(pFilter_SS, &QPushButton::released, this, [=]() {
        OnFilterChanged();
    });
    connect(pFilter_3B, &QPushButton::released, this, [=]() {
        OnFilterChanged();
    });
    connect(pFilter_OF, &QPushButton::released, this, [=]() {
        OnFilterChanged();
    });

    OnFilterChanged();

}

