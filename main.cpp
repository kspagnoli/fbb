#include <QApplication>
#include <QLabel>
#include <QTableView>
#include <QMainWindow>
#include <QStyleFactory>
#include <QHeaderView>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QSplitter>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpacerItem>
#include <QCompleter>
#include <QPushButton>
#include <QItemDelegate>
#include <QMessageBox>
#include <QListWidget>
#include <QStackedWidget>
#include <QComboBox>
#include <QMessageBox>
#include <QSettings>
#include <QDataStream>
#include <QBuffer>
#include <QToolTip>

#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "DraftDialog.h"
#include "PlayerTableModel.h"
#include "PlayerSortFilterProxyModel.h"
#include "SelectedPlayer.h"
#include "PlayerAppearances.h"
#include "DraftDelegate.h"
#include "OwnerSortFilterProxyModel.h"
#include "PlayerScatterPlotChart.h"

// TODO: mve to settings
static const uint32_t NUM_OWNERS = 9;


static QString TableStyle()
{
    return R"""(
        QTableView, QHeaderView, QToolTip {
            font-family: "Consolas";
            font-size: 11px;
        }
        )""";
}

//----------------------------------------------------------------------
// Summary
//----------------------------------------------------------------------

class SummaryWidget : public QWidget
{

public:

    SummaryWidget(uint32_t numOwners, QWidget* parent)
        : QWidget(parent)
        , m_tableModel(new SummaryTableModel(numOwners, parent))
        , m_tableView(new QTableView)
        , m_layout(new QVBoxLayout)
    {

        // widget layout
        setLayout(m_layout);

        // table view
        m_tableView->setModel(m_tableModel);
        m_tableView->verticalHeader()->hide();
        m_tableView->setStyleSheet(TableStyle());
        m_tableView->setAlternatingRowColors(true);
        m_tableView->verticalHeader()->setDefaultSectionSize(15);
        m_tableView->horizontalHeader()->setDefaultSectionSize(50);
        m_tableView->setCornerButtonEnabled(true);
        m_layout->addWidget(m_tableView);
    }

public slots:

    void OnDrafted(const DraftDialog::Results& results, const QModelIndex& index, QAbstractItemModel* model)
    {
        auto GetValue = [=](uint32_t column)
        {
            QModelIndex indexHR = model->index(index.row(), column);
            return model->data(indexHR, PlayerTableModel::RawDataRole).toUInt();
        };

        // Hitting
        // uint32_t AB = 0;
        // uint32_t H = 0;
        // uint32_t R = 0;
        // uint32_t HR = 0;
        // uint32_t RBI = 0;
        // uint32_t SB = 0;
        // uint32_t IP = 0;
        // uint32_t HA = 0;
        // uint32_t BB = 0;
        // uint32_t W = 0;
        // uint32_t K = 0;
        // uint32_t S = 0;
        // uint32_t SUM;

        // Update values
        m_tableModel->m_vecOwnerPoints[results.ownerId].AB  += GetValue(PlayerTableModel::COLUMN_AB);
        // m_tableModel->m_vecOwnerPoints[results.ownerId].H   += GetValue(PlayerTableModel::COLUMN_H);
        m_tableModel->m_vecOwnerPoints[results.ownerId].R   += GetValue(PlayerTableModel::COLUMN_R);
        m_tableModel->m_vecOwnerPoints[results.ownerId].RBI += GetValue(PlayerTableModel::COLUMN_RBI);
        m_tableModel->m_vecOwnerPoints[results.ownerId].SB  += GetValue(PlayerTableModel::COLUMN_SB);

        m_tableModel->m_vecOwnerPoints[results.ownerId].IP  += GetValue(PlayerTableModel::COLUMN_IP);
        // m_tableModel->m_vecOwnerPoints[results.ownerId].HA  += GetValue(PlayerTableModel::COLUMN_HA);
        // m_tableModel->m_vecOwnerPoints[results.ownerId].BB  += GetValue(PlayerTableModel::COLUMN_BB);
        m_tableModel->m_vecOwnerPoints[results.ownerId].W  += GetValue(PlayerTableModel::COLUMN_W);
        m_tableModel->m_vecOwnerPoints[results.ownerId].K  += GetValue(PlayerTableModel::COLUMN_SO);
        m_tableModel->m_vecOwnerPoints[results.ownerId].S  += GetValue(PlayerTableModel::COLUMN_SV);
    }

private:

    class SummaryTableModel : public QAbstractTableModel
    {

        friend SummaryWidget;

    public:

        SummaryTableModel(uint32_t numOwners, QWidget* parent)
            : QAbstractTableModel(parent)
            , m_vecOwnerPoints(numOwners)
        {
        }

        enum RankRows
        {
            TEAM,
            BA,
            R,
            HR,
            RBI,
            SB,
            ERA,
            WHIP,
            W,
            K,
            S,
            SUM,

            COUNT
        };

        virtual int rowCount(const QModelIndex& index) const override
        {
            return int(m_vecOwnerPoints.size());
        }

        virtual int columnCount(const QModelIndex& index) const override
        {
            return RankRows::COUNT;
        }

        virtual QVariant data(const QModelIndex& index, int role) const override
        {
            if (role != Qt::DisplayRole) {
                return QVariant();
            }

            const OwnerPoints& player = m_vecOwnerPoints.at(index.row());

            switch (index.column())
            {
            case RankRows::TEAM:
                return index.row();
            case RankRows::BA:
                return player.AB == 0 ? "--" : QString::number(player.H / float(player.AB), 'f', 3);
            case RankRows::R:
                return player.R;
            case RankRows::HR:
                return player.HR;
            case RankRows::RBI:
                return player.RBI;
            case RankRows::SB:
                return player.SB;
            case RankRows::ERA:
                return player.IP == 0 ? "--" : QVariant(0);
            case RankRows::WHIP:
                return player.IP == 0 ? "--" : QVariant(0);
            case RankRows::W:
                return player.W;
            case RankRows::K:
                return player.K;
            case RankRows::S:
                return player.S;
            case RankRows::SUM:
                return player.SUM;
            }

            return QVariant();
        }

        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override
        {
            // Only care about display roles
            if (role != Qt::DisplayRole) {
                return QVariant();
            }

            if (orientation == Qt::Horizontal) {

                switch (section)
                {
                case RankRows::TEAM:
                    return "Team";
                case RankRows::BA:
                    return "AVG";
                case RankRows::R:
                    return "R";
                case RankRows::HR:
                    return "HR";
                case RankRows::RBI:
                    return "RBI";
                case RankRows::SB:
                    return "SB";
                case RankRows::ERA:
                    return "ERA";
                case RankRows::WHIP:
                    return "WHIP";
                case RankRows::W:
                    return "W";
                case RankRows::K:
                    return "K";
                case RankRows::S:
                    return "S";
                case RankRows::SUM:         // TODO: sigma sign
                    return "SUM";
                default:
                    break;
                }
            }

            return QVariant();
        }

    private:

        struct OwnerPoints
        {
            // Hitting
            uint32_t AB = 0;
            uint32_t H = 0;
            uint32_t R = 0;
            uint32_t HR = 0;
            uint32_t RBI = 0;
            uint32_t SB = 0;

            // Pitching
            uint32_t IP = 0;
            uint32_t HA = 0;
            uint32_t BB = 0;
            uint32_t W = 0;
            uint32_t K = 0;
            uint32_t S = 0;

            // Summary
            uint32_t SUM;
        };

        std::vector<OwnerPoints> m_vecOwnerPoints;
    };

    QVBoxLayout* m_layout;
    SummaryTableModel* m_tableModel;
    QTableView* m_tableView;
};

//class OwnerWidget 
//{
//public:
//
//    OwnerWidget(QWidget* parent)
//        : m_model(new OwnerWidgetItemModel(parent))
//        , m_tableView(new QTableView(parent))
//    {
//    }
//
//    QTableView* table()
//    {
//        return m_tableView;
//    }
//
//private:
//
//    class OwnerWidgetItemModel : public QAbstractTableModel
//    {
//    public:
//
//        OwnerWidgetItemModel(QWidget* parent)
//            : QAbstractTableModel(parent)
//        {
//        }
//
//        virtual int rowCount(const QModelIndex& index) const override
//        {
//            return 0;
//        }
//
//        virtual int columnCount(const QModelIndex& index) const override
//        {
//            return Columns::COUNT;
//        }
//
//        virtual QVariant data(const QModelIndex& index, int role) const override
//        {
//            return QVariant();
//        }
//
//        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override
//        {
//            if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
//
//                switch (section)
//                {
//                case COLUMN_DRAFT_POSITION:
//                    return "Pos.";
//                case COLUMN_PAID:
//                    return "$";
//                case COLUMN_NAME:
//                    return "Name";
//                }
//            }
//
//            return QVariant();
//        }
//
//    private:
//
//        enum Columns {
//            COLUMN_DRAFT_POSITION,
//            COLUMN_PAID,
//            COLUMN_NAME,
//            COUNT,
//        };
//    };
//
//    struct OwnedPlayer
//    {
//        QString position;
//        uint32_t cost;
//        QString name;
//    };
//
//    OwnedPlayer
//
//    OwnerWidgetItemModel* m_model;
//    QTableView* m_tableView;
//};

//
class MainWindow : public QMainWindow
{

public:

    MainWindow()
    {
        // Settings persistence
        ReadSettings();

        // Appearance LUT
        PlayerApperances appearances("2015_appearances.csv");

        // Draft delegate
        DraftDelegate* draftDelegate = new DraftDelegate(this);
        connect(draftDelegate, &DraftDelegate::Drafted, [=](const DraftDialog::Results& results, const QModelIndex& index, QAbstractItemModel* model) {

            // Player row
            uint32_t row = index.row();

            // Update status
            model->setData(index, uint32_t(Player::Status::Drafted));

            // Update owner
            uint32_t ownerColumn = PlayerTableModel::COLUMN_OWNER;
            QModelIndex ownerIndex = model->index(row, ownerColumn);
            model->setData(ownerIndex, results.ownerId);

            // Update paid amount
            uint32_t paidColumn = PlayerTableModel::COLUMN_PAID;
            QModelIndex paidIndex = model->index(row, paidColumn);
            model->setData(paidIndex, results.cost);

            // Update position 
            uint32_t draftPositionColumn = PlayerTableModel::COLUMN_DRAFT_POSITION;
            QModelIndex draftPositionIndex = model->index(row, draftPositionColumn);
            model->setData(draftPositionIndex, results.position);
        });

        // Player table model
        PlayerTableModel* playerTableModel = new PlayerTableModel(this);
        playerTableModel->LoadHittingProjections("2015_hitters.csv", appearances);
        playerTableModel->LoadPitchingProjections("2015_pitchers.csv", appearances);
        playerTableModel->AddDummyPositions();

        // Hitter sort-model
        PlayerSortFilterProxyModel* hitterSortFilterProxyModel = new PlayerSortFilterProxyModel(Player::Hitter);
        hitterSortFilterProxyModel->setSourceModel(playerTableModel);
        hitterSortFilterProxyModel->setSortRole(PlayerTableModel::RawDataRole);

        // Hitter table view
        QTableView* hitterTableView = MakeTableView(hitterSortFilterProxyModel, true, PlayerTableModel::COLUMN_Z);
        hitterTableView->setItemDelegateForColumn(FindColumn(hitterSortFilterProxyModel, PlayerTableModel::COLUMN_DRAFT_BUTTON), draftDelegate);
        hitterTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

        // Pitcher sort-model
        PlayerSortFilterProxyModel* pitcherSortFilterProxyModel = new PlayerSortFilterProxyModel(Player::Pitcher);
        pitcherSortFilterProxyModel->setSourceModel(playerTableModel);
        pitcherSortFilterProxyModel->setSortRole(PlayerTableModel::RawDataRole);
        
        // Pitcher table view
        QTableView* pitcherTableView = MakeTableView(pitcherSortFilterProxyModel, true, PlayerTableModel::COLUMN_Z);
        pitcherTableView->setItemDelegateForColumn(FindColumn(pitcherSortFilterProxyModel, PlayerTableModel::COLUMN_DRAFT_BUTTON), draftDelegate);
        pitcherTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

        // Top/Bottom splitter
        QSplitter* topBottomSplitter = new QSplitter(Qt::Vertical);
        topBottomSplitter->setContentsMargins(5, 5, 5, 5);

        // Hitter/Pitcher tab View
        enum PlayerTableTabs { Hitters, Pitchers, Unknown };
        QTabWidget* hitterPitcherTabs = new QTabWidget(this);
        hitterPitcherTabs->insertTab(PlayerTableTabs::Hitters, hitterTableView, "Hitters");
        hitterPitcherTabs->insertTab(PlayerTableTabs::Pitchers, pitcherTableView, "Pitchers");
        topBottomSplitter->addWidget(hitterPitcherTabs);

        // Tab lookup helper
        auto CaterogyToTab = [](uint32_t catergory) 
        {
            switch (catergory)
            {
            case Player::Hitter:
                return PlayerTableTabs::Hitters;
            case Player::Pitcher:
                return PlayerTableTabs::Pitchers;
            default:
                return PlayerTableTabs::Unknown;
            }
        };

        // NL filter action
        QAction* filterNL = new QAction(this);
        connect(filterNL, &QAction::toggled, hitterSortFilterProxyModel, &PlayerSortFilterProxyModel::OnFilterNL);
        connect(filterNL, &QAction::toggled, pitcherSortFilterProxyModel, &PlayerSortFilterProxyModel::OnFilterNL);
        filterNL->setText(tr("NL"));
        filterNL->setToolTip("Toggle National Leauge");
        filterNL->setCheckable(true);
        filterNL->toggle();

        // AL filter action
        QAction* filterAL = new QAction(this);
        connect(filterAL, &QAction::toggled, hitterSortFilterProxyModel, &PlayerSortFilterProxyModel::OnFilterAL);
        connect(filterAL, &QAction::toggled, pitcherSortFilterProxyModel, &PlayerSortFilterProxyModel::OnFilterAL);
        filterAL->setText(tr("AL"));
        filterAL->setToolTip("Toggle American Leauge");
        filterAL->setCheckable(true);
        filterAL->toggle();

        // FA filter action
        QAction* filterFA = new QAction(this);
        connect(filterFA, &QAction::toggled, hitterSortFilterProxyModel, &PlayerSortFilterProxyModel::OnFilterFA);
        connect(filterFA, &QAction::toggled, pitcherSortFilterProxyModel, &PlayerSortFilterProxyModel::OnFilterFA);
        filterFA->setText(tr("FA"));
        filterFA->setToolTip("Toggle Free Agents");
        filterFA->setCheckable(true);
        filterAL->toggle();
        filterAL->toggle();

        // General filter group
        QActionGroup* generalFilters = new QActionGroup(this);
        generalFilters->addAction(filterAL);
        generalFilters->addAction(filterNL);
        generalFilters->addAction(filterFA);
        generalFilters->setExclusive(false);

        // Starter filter action
        QAction* filterStarter = new QAction(this);
        connect(filterStarter, &QAction::toggled, pitcherSortFilterProxyModel, &PlayerSortFilterProxyModel::OnFilterSP);
        filterStarter->setText(tr("SP"));
        filterStarter->setToolTip("Toggle Starting Pitchers");
        filterStarter->setCheckable(true);
        filterStarter->toggle();

        // Relief filter action
        QAction* filterRelief = new QAction(this);
        connect(filterRelief, &QAction::toggled, pitcherSortFilterProxyModel, &PlayerSortFilterProxyModel::OnFilterRP);
        filterRelief->setText(tr("RP"));
        filterRelief->setToolTip("Toggle Relief Pitchers");
        filterRelief->setCheckable(true);
        filterRelief->toggle();

        // Pitching filter group
        QActionGroup* pitchingFilters = new QActionGroup(this);
        pitchingFilters->addAction(filterStarter);
        pitchingFilters->addAction(filterRelief);
        pitchingFilters->setExclusive(false);

        // Hitting filter group
        QActionGroup* hittingFilters = new QActionGroup(this);
        hittingFilters->setExclusive(false);

        // Filter helper
        auto MakeHitterFilter = [=](QString text, QString toolTip, const auto& onFilterFn) -> QAction* 
        {
            QAction* action = new QAction(this);
            connect(action, &QAction::toggled, hitterSortFilterProxyModel, onFilterFn);
            action->setText(text);
            action->setToolTip(toolTip);
            action->setCheckable(true);
            action->toggle();
            hittingFilters->addAction(action);

            return action;
        };

        // Hitter filters
        QAction* filterC  = MakeHitterFilter("C",  "Filter Catchers",           &PlayerSortFilterProxyModel::OnFilterC);
        QAction* filter1B = MakeHitterFilter("1B", "Filter 1B",                 &PlayerSortFilterProxyModel::OnFilter1B);
        QAction* filter2B = MakeHitterFilter("2B", "Filter 2B",                 &PlayerSortFilterProxyModel::OnFilter2B);
        QAction* filterSS = MakeHitterFilter("SS", "Filter SS",                 &PlayerSortFilterProxyModel::OnFilterSS);
        QAction* filter3B = MakeHitterFilter("3B", "Filter 3B",                 &PlayerSortFilterProxyModel::OnFilter3B);
        QAction* filterOF = MakeHitterFilter("OF", "Filter Outfielders",        &PlayerSortFilterProxyModel::OnFilterOF);
        QAction* filterDH = MakeHitterFilter("DH", "Filter Designated Hitters", &PlayerSortFilterProxyModel::OnFilterDH);

        // Menu spacer
        QWidget* spacer = new QWidget(this);
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // Completion Widget
        QCompleter* completer = new QCompleter(this);
        completer->setModel(playerTableModel);
        completer->setCompletionColumn(PlayerTableModel::COLUMN_NAME);
        completer->setFilterMode(Qt::MatchContains);
        completer->setCaseSensitivity(Qt::CaseInsensitive);

        // Select
        auto HighlightPlayerInTable = [=](const QModelIndex& index)
        {
            // Lookup catergory
            auto catergoryIndex = playerTableModel->index(index.row(), PlayerTableModel::COLUMN_CATERGORY);
            auto catergory = playerTableModel->data(catergoryIndex, PlayerTableModel::RawDataRole).toUInt();

            // Change to tab
            hitterPitcherTabs->setCurrentIndex(CaterogyToTab(catergory));

            // Select row
            if (catergory == Player::Catergory::Hitter) {
                auto row = hitterSortFilterProxyModel->mapFromSource(index).row();
                hitterTableView->selectRow(row);
                hitterTableView->setFocus();
            } else if (catergory == Player::Catergory::Pitcher) {
                auto row = pitcherSortFilterProxyModel->mapFromSource(index).row();
                pitcherTableView->selectRow(row);
                pitcherTableView->setFocus();
            }
        };

        // Select the target 
        connect(completer, static_cast<void (QCompleter::*)(const QModelIndex&)>(&QCompleter::activated), [=](const QModelIndex& index) {

            // Get player index
            int key = completer->completionModel()->index(index.row(), 0).data().toInt();
            QModelIndex sourceIdx = playerTableModel->index(key, 1);

            // Highlight this player
            HighlightPlayerInTable(sourceIdx);
        });

        // Search widget
        QLineEdit* playerSearch = new QLineEdit(this);
        playerSearch->setCompleter(completer);

        // Main toolbar
        QToolBar* toolbar = new QToolBar("Toolbar");
        toolbar->addWidget(new QLabel(" Leagues: ", this));
        toolbar->addActions(QList<QAction*>{filterAL, filterNL, filterFA});
        toolbar->addSeparator();
        toolbar->addWidget(new QLabel(" Positions: ", this));
        toolbar->addActions(QList<QAction*>{filterStarter, filterRelief});
        toolbar->addActions(QList<QAction*>{filterC, filter1B, filter2B, filterSS, filter3B, filterOF, filterDH});
        toolbar->addWidget(spacer);
        toolbar->addWidget(new QLabel("Player Search: ", this));
        toolbar->addWidget(playerSearch);
        toolbar->setFloatable(false);
        toolbar->setMovable(false);
        QMainWindow::addToolBar(toolbar);

        // Helper to adjust filters
        auto ToggleFilterGroups = [=](int index)
        {
            switch (index)
            {
            case uint32_t(PlayerTableTabs::Hitters):
                pitchingFilters->setVisible(false);
                hittingFilters->setVisible(true);
                break;
            case uint32_t(PlayerTableTabs::Pitchers):
                pitchingFilters->setVisible(true);
                hittingFilters->setVisible(false);
                break;
            default:
                break;
            }
        };

        // Set default filter group
        ToggleFilterGroups(hitterPitcherTabs->currentIndex());

        //---------------------------------------------------------------------
        // Bottom Section
        //---------------------------------------------------------------------

        // H-Layout for all owners
        QHBoxLayout* ownersLayout = new QHBoxLayout(this);

        // Loop owners
        for (int ownerId = 1; ownerId <= NUM_OWNERS; ownerId++) {

            // V-Layout per owner
            QVBoxLayout* perOwnerLayout = new QVBoxLayout(this);

            // Owner name label
            QLabel* ownerLabel = new QLabel(QString("Owner #%1").arg(ownerId), this);
            ownerLabel->setAlignment(Qt::AlignCenter);
            perOwnerLayout->addWidget(ownerLabel);

            // Table view
            OwnerSortFilterProxyModel* ownerSortProxyModel = new OwnerSortFilterProxyModel(ownerId);
            ownerSortProxyModel->setSourceModel(playerTableModel);
            QTableView* ownerTableView = MakeTableView(ownerSortProxyModel, false);
            ownerTableView->setFixedWidth(200);
            perOwnerLayout->addWidget(ownerTableView);

            // Summary labels
            QLabel* summary = new QLabel( "Total: $0", this);
            perOwnerLayout->addWidget(summary);
  
            // When drafted...
            connect(draftDelegate, &DraftDelegate::Drafted, ownerSortProxyModel, &OwnerSortFilterProxyModel::OnDrafted);
            connect(draftDelegate, &DraftDelegate::Drafted, [=](const DraftDialog::Results& results, const QModelIndex& index, QAbstractItemModel* model) {
                uint32_t total = ownerSortProxyModel->GetTotalSpent();
                summary->setText(QString("Total: $%1").arg(total));
            });
            
            // Add to layouts
            ownersLayout->addLayout(perOwnerLayout);
        }

        // Owner widget
        QWidget* ownerWidget = new QWidget(this);
        ownerWidget->setLayout(ownersLayout);

        // OwnerScrollArea sizeHint helper
        class OwnerScrollArea : public QScrollArea {
        public:
            OwnerScrollArea(QWidget* parent) : QScrollArea(parent) {
                setFrameShape(QFrame::NoFrame);
            }
            virtual QSize sizeHint() const override { return QSize(200, 260); }
        };

        // Owner scroll area
        OwnerScrollArea* ownerScrollArea = new OwnerScrollArea(this);
        ownerScrollArea->setWidget(ownerWidget);
        ownerScrollArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        
        // Player scatter plot
        PlayerScatterPlotChart* chartView = new PlayerScatterPlotChart(playerTableModel, hitterSortFilterProxyModel, this);
        connect(hitterSortFilterProxyModel,  &QSortFilterProxyModel::layoutChanged, chartView, &PlayerScatterPlotChart::Update);
        connect(pitcherSortFilterProxyModel, &QSortFilterProxyModel::layoutChanged, chartView, &PlayerScatterPlotChart::Update);
        connect(playerTableModel, &QAbstractItemModel::dataChanged, chartView, &PlayerScatterPlotChart::Update);

        // Summary
        SummaryWidget* summary = new SummaryWidget(NUM_OWNERS, this);

        // Bottom tabs
        enum BottomSectionTabs { Rosters, SummaryWidget, ChartVie };
        QTabWidget* bottomTabs = new QTabWidget(this);
        bottomTabs->insertTab(BottomSectionTabs::Rosters, ownerScrollArea, "Rosters");
        bottomTabs->insertTab(BottomSectionTabs::SummaryWidget, summary, "Summary");
        bottomTabs->insertTab(BottomSectionTabs::ChartVie, chartView, "Scatter Chart");
        topBottomSplitter->addWidget(bottomTabs);

        // Make top section 3x the size of the bottom
        topBottomSplitter->setStretchFactor(0, 3);
        topBottomSplitter->setStretchFactor(1, 1);

        //----------------------------------------------------------------------
        // Connections
        //----------------------------------------------------------------------

        // Connect tab filters
        connect(hitterPitcherTabs, &QTabWidget::currentChanged, this, [=](int index) {
            
            // Update filters
            ToggleFilterGroups(index);

            // Update chart view
            switch (index)
            {
            case PlayerTableTabs::Hitters:
                chartView->SetProxyModel(hitterSortFilterProxyModel);
                break;
            case PlayerTableTabs::Pitchers:
                chartView->SetProxyModel(pitcherSortFilterProxyModel);
                break;
            default:
                break;
            }
        });

        // Connect chart click
        connect(chartView, &PlayerScatterPlotChart::PlayerClicked, this, [=](const QModelIndex& index) {
            HighlightPlayerInTable(index);
        });

        connect(draftDelegate, &DraftDelegate::Drafted, summary, &SummaryWidget::OnDrafted);

        //----------------------------------------------------------------------
        // Main
        //----------------------------------------------------------------------

        // Set as main window
        QMainWindow::setCentralWidget(topBottomSplitter);

        // Create menu bar
        QMenuBar* menuBar = new QMenuBar();
        menuBar->addAction("MENU BAR");
        QMainWindow::setMenuBar(menuBar);

        // show me
        QMainWindow::show();
    }

private:

    // Owner names
    QVector<QString> m_vecOwners;

    static std::unique_ptr<QSettings> Settings()
    {
        return std::make_unique<QSettings>(QSettings::IniFormat, QSettings::UserScope, "SpagTech", "FbbDemo");
    }

    void WriteSettings() const
    {
        auto settings = Settings();

        // Main window
        settings->beginGroup("MainWindow");
        settings->setValue("size", size());
        settings->setValue("pos", pos());
        settings->endGroup();
    }

    void ReadSettings()
    {
        auto settings = Settings();

        // Main window
        settings->beginGroup("MainWindow");
        resize(settings->value("size", QSize(400, 400)).toSize());
        move(settings->value("pos", QPoint(200, 200)).toPoint());
        settings->endGroup();
    }

    void closeEvent(QCloseEvent* event) override
    {
        WriteSettings();
    }

    // Table factory
    QTableView* MakeTableView(QAbstractItemModel* model, bool sortingEnabled = true, uint32_t modelSortColumn = 0)
    {
        QTableView* tableView = new QTableView();
        tableView->setModel(model);
        tableView->setSortingEnabled(sortingEnabled);
        if (sortingEnabled) {
            tableView->sortByColumn(FindColumn(model, modelSortColumn));
        }
        tableView->verticalHeader()->hide();
        tableView->setStyleSheet(TableStyle());
        tableView->setAlternatingRowColors(true);
        tableView->verticalHeader()->setDefaultSectionSize(15);
        tableView->resizeColumnsToContents();
        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView->setFocusPolicy(Qt::StrongFocus);
        return tableView;
    }

    static int32_t FindColumn(QAbstractItemModel* model, uint32_t column)
    {
        for (int32_t i = 0; i < model->columnCount(); i++) {
            uint32_t id = model->headerData(i, Qt::Horizontal, Qt::InitialSortOrderRole).toUInt();
            if (id == column) {
                return i;
            }
        }

        return -1;
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyle(QStyleFactory::create("Fusion"));

    /*
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(darkPalette);
    */

    app.setStyleSheet(R"""(
        QToolTip { 
            color: #ffffff; 
            background-color: #2a82da; 
            border: 1px solid white; 
        }
        )""");

    MainWindow mainWin;
    mainWin.resize(1000, 800);
    mainWin.show();

    return app.exec();
}

