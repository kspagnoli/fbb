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
#include <QComboBox>

enum
{
    FBB_POSITION_ALL_HITTERS  = -1,
    FBB_POSITION_ALL_PITCHERS = -2,
};

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

    // Positions
    QComboBox* pFilter = new QComboBox(this);
    pHeaderLayout->addWidget(pFilter);
    auto BuildPositionList = [=]()
    {
        const FBBPositionMask mask = fbbApp->Settings()->EnabledPositions();
        
        pFilter->addItem("All", mask);
        pFilter->insertSeparator(INT_MAX);
        pFilter->addItem("Hitters", FBB_POSITION_ALL_HITTERS);
        pFilter->addItem("Pitchers", FBB_POSITION_ALL_PITCHERS);
        pFilter->insertSeparator(INT_MAX);

        for (int i = 0; i <= FBBPositionBitCount; i++)
        {
            int bit = 1 << i;
            if (bit & mask)
            {
                FBBPositionBits pos = static_cast<FBBPositionBits>(bit);
                QString strPos = FBBPositionToString(pos);
                pFilter->addItem(strPos, bit);
            }
        }
    };
    BuildPositionList();

    // Divider
    pHeaderLayout->addWidget(new VDivider(this));


    // Filter drafter button
    QPushButton* pFilter_Drafted = new QPushButton("Drafted", this);
    pFilter_Drafted->setCheckable(true);
    pFilter_Drafted->setChecked(true);
    pHeaderLayout->addWidget(pFilter_Drafted);

    // Divider
    pHeaderLayout->addWidget(new VDivider(this));

    // Z/$ button
    QPushButton* pZscoreToggle = new QPushButton("#/z");
    pZscoreToggle->setCheckable(true);
    pHeaderLayout->addWidget(pZscoreToggle, 0, 0);

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
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_Z_AVG,      QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_Z_HR,       QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_Z_R,        QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_Z_RBI,      QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_Z_SB,       QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_IP,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_HA,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_BB,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_ER,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_SO,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_ERA,        QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_WHIP,       QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_W,          QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_SV,         QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_Z_SO,       QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_Z_ERA,      QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_Z_WHIP,     QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_Z_W,        QHeaderView::Fixed);
    pTableView->horizontalHeader()->setSectionResizeMode(FBBDraftBoardModel::COLUMN_Z_SV,       QHeaderView::Fixed);
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
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_Z_AVG,      charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_Z_HR,       charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_Z_R,        charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_Z_RBI,      charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_Z_SB,       charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_IP,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_HA,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_BB,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_ER,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_SO,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_ERA,        charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_WHIP,       charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_W,          charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_SV,         charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_Z_SO,       charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_Z_ERA,      charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_Z_WHIP,     charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_Z_W,        charWidth * 7 + padding);
    pTableView->setColumnWidth(FBBDraftBoardModel::COLUMN_Z_SV,       charWidth * 7 + padding);
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
            pDraftButton->setText(QString("Draft %1").arg(name));
        } else {
            pDraftButton->setText("Draft");
        }
    });

    // Draft button interactions
    connect(pDraftButton, &QPushButton::released, this, [=]() {
        const QModelIndex srcIdx = pProxyModel->mapToSource(pTableView->selectionModel()->currentIndex());
        FBBPlayer* pPlayer = fbbApp->DraftBoardModel()->GetPlayer(srcIdx.row());
        FBBDraftDialog dialog(pPlayer);
        dialog.exec();
    });

    // Handle zScores
    auto UpdateZScoreToggle = [=]()
    {
        const bool showZ = pZscoreToggle->isChecked();

        for (int i = FBBDraftBoardModel::COLUMN_FIRST_HITTING; i <= FBBDraftBoardModel::COLUMN_LAST_HITTING; i++) {
            pTableView->setColumnHidden(i, showZ);
        }

        for (int i = FBBDraftBoardModel::COLUMN_FIRST_Z_HITTING; i <= FBBDraftBoardModel::COLUMN_LAST_Z_HITTING; i++) {
            pTableView->setColumnHidden(i, !showZ);
        }

        for (int i = FBBDraftBoardModel::COLUMN_FIRST_PITCHING; i <= FBBDraftBoardModel::COLUMN_LAST_PITCHING; i++) {
            pTableView->setColumnHidden(i, showZ);
        }

        for (int i = FBBDraftBoardModel::COLUMN_FIRST_Z_PITCHING; i <= FBBDraftBoardModel::COLUMN_LAST_Z_PITCHING; i++) {
            pTableView->setColumnHidden(i, !showZ);
        }
    };
    connect(pZscoreToggle, &QPushButton::toggled, this, [=](bool checked) {
        UpdateZScoreToggle();
    });
    UpdateZScoreToggle();

    // Search activation
    connect(pSearchCompleter, static_cast<void(QCompleter::*)(const QModelIndex&)>(&QCompleter::activated), this, [=](const QModelIndex& index) {
        QAbstractProxyModel* pCompletionModel = reinterpret_cast<QAbstractProxyModel*>(pSearchCompleter->completionModel());
        const QModelIndex proxyIndex = pCompletionModel->mapToSource(index);
        pTableView->selectRow(proxyIndex.row());
        pTableView->setFocus();
    });

    //
    auto OnFilterChanged = [=]()
    {
        const int mask = pFilter->currentData().toInt();
        pProxyModel->SetOnlyPitchers(mask == FBB_POSITION_ALL_PITCHERS);
        pProxyModel->SetOnlyHitters(mask == FBB_POSITION_ALL_HITTERS);
        pProxyModel->SetPositionFilter(mask);
        pProxyModel->SetShowDrafted(pFilter_Drafted->isChecked());
    };

    connect(pFilter, &QComboBox::currentTextChanged, this, [=](const QString& text) {
        OnFilterChanged();
    });

    connect(pFilter_Drafted, &QPushButton::released, this, [=]() {
        OnFilterChanged();
    });

    OnFilterChanged();

}

