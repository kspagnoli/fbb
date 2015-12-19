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

#include <memory>
#include <string>
#include <vector>

#include "DraftDialog.h"
#include "Hitter.h"
#include "HitterTableModel.h"
#include "HitterSortFilterProxyModel.h"
#include "Pitcher.h"
#include "PitcherTableModel.h"
#include "PitcherSortFilterProxyModel.h"
#include "SelectedPlayer.h"
#include "PlayerAppearances.h"

class DraftDelegate : public QAbstractItemDelegate
{
public:

    DraftDelegate(QWidget* parent)
        : QAbstractItemDelegate(parent)
        , m_parent(parent)
    {
    }

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        Player::Status status = Player::Status(index.data().toInt());

        option.state;

        // Player is available
        if (status != Player::Status::Drafted) {

            QStyleOptionButton pushButtonOption;
            pushButtonOption.rect = option.rect;
            pushButtonOption.text = "Draft";
            pushButtonOption.state = option.state != QStyle::State_Selected ? QStyle::State_Raised : QStyle::State_Sunken; // [XXX] does nothing!

            QApplication::style()->drawControl(QStyle::CE_PushButton, &pushButtonOption, painter);
        } 
    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        return QSize(option.rect.width(), option.rect.height());
    }

    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override
    {
        // Skip if already drafted
        if (Player::Status(index.data().toInt()) == Player::Status::Drafted) {
            return false;
        }

        // Otherwise create a draft dialog
        if (event->type() == QEvent::MouseButtonRelease) {

            DraftDialog* draftDialog = new DraftDialog(model, index);
            draftDialog->show();
            draftDialog->raise();
            draftDialog->activateWindow();

            // On draft
            connect(draftDialog, &QDialog::accepted, [=]() -> void {

                // Get results
                const DraftDialog::Results& results = draftDialog->GetDraftResults();

                // Update draft status
                model->setData(index, uint32_t(Player::Status::Drafted));
            });

            return true;
        }

        return false;
    }

private:

    QWidget* m_parent;
};

//
class MainWindow : public QMainWindow
{

public:

    MainWindow()
    {
        PlayerApperances appearances("2015_appearances.csv");

        // hitter table
        HitterTableModel* hitterTableModel = new HitterTableModel("2015_hitters.csv", appearances, this);
        HitterSortFilterProxyModel* hitterSortFilterProxyModel = new HitterSortFilterProxyModel();
        hitterSortFilterProxyModel->setSourceModel(hitterTableModel);
        hitterSortFilterProxyModel->setSortRole(HitterTableModel::RawDataRole);
        QTableView* hitterTableView = MakeTableView(hitterSortFilterProxyModel, 0);
        hitterTableView->setItemDelegateForColumn(HitterTableModel::COLUMN_DRAFT_STATUS, new DraftDelegate(this));
        hitterTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

        // pitcher table
        PitcherTableModel* pitcherTableModel = new PitcherTableModel("2015_pitchers.csv", appearances, this);
        PitcherSortFilterProxyModel* pitcherSortFilterProxyModel = new PitcherSortFilterProxyModel();
        pitcherSortFilterProxyModel->setSourceModel(pitcherTableModel);
        pitcherSortFilterProxyModel->setSortRole(PitcherTableModel::RawDataRole);
        QTableView* pitcherTableView = MakeTableView(pitcherSortFilterProxyModel, 0);
        pitcherTableView->setItemDelegateForColumn(PitcherTableModel::COLUMN_DRAFT_STATUS, new DraftDelegate(this));
        pitcherTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

        // layout
        QVBoxLayout* vBoxLayout = new QVBoxLayout();

        // selected player widget
        // SelectedPlayer* selectedPlayer = new SelectedPlayer(this);
        // selectedPlayer->setFixedHeight(100);
        // vBoxLayout->addWidget(selectedPlayer);

        enum class Tabs
        {
            Hitters,
            Pitchers,
        };

        // hitter/pitcher tab View
        QTabWidget* tabs = new QTabWidget(this);
        tabs->insertTab(int(Tabs::Hitters), hitterTableView, "Hitters");
        tabs->insertTab(int(Tabs::Pitchers), pitcherTableView, "Pitchers");
        vBoxLayout->addWidget(tabs);

        // NL filter action
        QAction* filterNL = new QAction(this);
        connect(filterNL, &QAction::toggled, hitterSortFilterProxyModel, &HitterSortFilterProxyModel::OnFilterNL);
        connect(filterNL, &QAction::toggled, pitcherSortFilterProxyModel, &PitcherSortFilterProxyModel::OnFilterNL);
        filterNL->setText(tr("NL"));
        filterNL->setToolTip("Toggle National Leauge");
        filterNL->setCheckable(true);
        filterNL->toggle();

        // AL filter action
        QAction* filterAL = new QAction(this);
        connect(filterAL, &QAction::toggled, hitterSortFilterProxyModel, &HitterSortFilterProxyModel::OnFilterAL);
        connect(filterAL, &QAction::toggled, pitcherSortFilterProxyModel, &PitcherSortFilterProxyModel::OnFilterAL);
        filterAL->setText(tr("AL"));
        filterAL->setToolTip("Toggle American Leauge");
        filterAL->setCheckable(true);
        filterAL->toggle();

        // FA filter action
        QAction* filterFA = new QAction(this);
        connect(filterFA, &QAction::toggled, hitterSortFilterProxyModel, &HitterSortFilterProxyModel::OnFilterFA);
        connect(filterFA, &QAction::toggled, pitcherSortFilterProxyModel, &PitcherSortFilterProxyModel::OnFilterFA);
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
        connect(filterStarter, &QAction::toggled, pitcherSortFilterProxyModel, &PitcherSortFilterProxyModel::OnFilterSP);
        filterStarter->setText(tr("SP"));
        filterStarter->setToolTip("Toggle Starting Pitchers");
        filterStarter->setCheckable(true);
        filterStarter->toggle();

        // Relief filter action
        QAction* filterRelief = new QAction(this);
        connect(filterRelief, &QAction::toggled, pitcherSortFilterProxyModel, &PitcherSortFilterProxyModel::OnFilterRP);
        filterRelief->setText(tr("RP"));
        filterRelief->setToolTip("Toggle Relief Pitchers");
        filterRelief->setCheckable(true);
        filterRelief->toggle();

        // Pictching filter group
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
        QAction* filterC  = MakeHitterFilter("C",  "Filter Catchers",           &HitterSortFilterProxyModel::OnFilterC);
        QAction* filter1B = MakeHitterFilter("1B", "Filter 1B",                 &HitterSortFilterProxyModel::OnFilter1B);
        QAction* filter2B = MakeHitterFilter("2B", "Filter 2B",                 &HitterSortFilterProxyModel::OnFilter2B);
        QAction* filterSS = MakeHitterFilter("SS", "Filter SS",                 &HitterSortFilterProxyModel::OnFilterSS);
        QAction* filter3B = MakeHitterFilter("3B", "Filter 3B",                 &HitterSortFilterProxyModel::OnFilter3B);
        QAction* filterOF = MakeHitterFilter("OF", "Filter Outfielders",        &HitterSortFilterProxyModel::OnFilterOF);
        QAction* filterDH = MakeHitterFilter("DH", "Filter Designated Hitters", &HitterSortFilterProxyModel::OnFilterDH);

        // Menu spacer
        QWidget* spacer = new QWidget(this);
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // Completion Widget
        QCompleter* completer = new QCompleter(this);
        completer->setModel(hitterTableModel);
        completer->setCompletionColumn(HitterTableModel::COLUMN_NAME);
        completer->setFilterMode(Qt::MatchContains);

        // Select the target 
        connect(completer, static_cast<void (QCompleter::*)(const QModelIndex&)>(&QCompleter::activated), [=](const QModelIndex& index) {
            int key = completer->completionModel()->index(index.row(), 0).data().toInt();
            QModelIndex sourceIdx = hitterTableModel->index(key-1, 0);
            auto row = hitterSortFilterProxyModel->mapFromSource(sourceIdx).row();
            hitterTableView->selectRow(row);
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
            case uint32_t(Tabs::Hitters):
                pitchingFilters->setVisible(false);
                hittingFilters->setVisible(true);
                break;
            case uint32_t(Tabs::Pitchers):
                pitchingFilters->setVisible(true);
                hittingFilters->setVisible(false);
                break;
            default:
                break;
            }
        };

        // Connect tab filters
        connect(tabs, &QTabWidget::currentChanged, this, [=](int index)
        {
            ToggleFilterGroups(index);
        });

        // Set default filter group
        ToggleFilterGroups(tabs->currentIndex());

        // set as main window
        QWidget* central = new QWidget();
        QMainWindow::setCentralWidget(central);
        central->setLayout(vBoxLayout);

        // create menu bar
        QMenuBar* menuBar = new QMenuBar();
        menuBar->addAction("MENU BAR");
        QMainWindow::setMenuBar(menuBar);

        // show me
        QMainWindow::show();
    }

private:

    // Table factory
    QTableView* MakeTableView(QAbstractItemModel* model, int sortColumn)
    {
        QTableView* tableView = new QTableView();
        tableView->setModel(model);
        tableView->setSortingEnabled(true);
        tableView->verticalHeader()->hide();
        tableView->setStyleSheet(m_style);
        tableView->setAlternatingRowColors(true);
        tableView->verticalHeader()->setDefaultSectionSize(15);
        tableView->resizeColumnsToContents();
        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView->setFocusPolicy(Qt::StrongFocus);
        model->sort(sortColumn);
        return tableView;
    }

    const QString m_style =
        R"""(
        QTableView, QHeaderView, QToolTip {
            font-family: "Consolas";
            font-size: 11px;
        }
        )""";
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyle(QStyleFactory::create("Fusion"));

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

    app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

    MainWindow mainWin;
    mainWin.resize(1000, 800);
    mainWin.show();

    return app.exec();
}
