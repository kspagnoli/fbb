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

#include <memory>
#include <string>
#include <vector>

#include "Hitter.h"
#include "HitterTableModel.h"
#include "HitterSortFilterProxyModel.h"

#include "Pitcher.h"
#include "PitcherTableModel.h"
#include "PitcherSortFilterProxyModel.h"

#include "SelectedPlayer.h"

#include "PlayerAppearances.h"

template <typename FnGet, typename Player>
float average(const FnGet& fnGet, const std::vector<Player>& players)
{
    double sum = std::accumulate(std::begin(players), std::end(players), double(0), [&](double sum, const Player& x)
    {
        return sum + double(fnGet(x));
    });

    return sum / float(players.size());
}

template <typename FnGet, typename Player>
float stdev(const FnGet& fnGet, const float& average, const std::vector<Player>& players)
{
    double accum = 0;
    std::for_each(std::begin(players), std::end(players), [&](const Player& x)
    {
        accum += (fnGet(x) - average) * (fnGet(x) - average);
    });

    return std::sqrt(accum / double(players.size() - 1));
}

float score(float sample, float avg, float stdev)
{
    return (sample - avg) / stdev;
};

struct Settings
{
    static const size_t Owners = 12;
    static const size_t RosterMoney = 270;
    static const size_t RosterHitters = 14;
    static const size_t RosterPitchers = 10;
    static const size_t TotalHittersRostered = Owners*RosterHitters;
    static const size_t TotalPitchersRostered = Owners*RosterPitchers;
    static float HitterPitcherSplit() { return 0.667f; }
    static float HitterMoney() { return float(RosterMoney) * HitterPitcherSplit(); }
    static float PitcherMoney() { return RosterMoney * (1.f - HitterPitcherSplit()); }
};



/*

// UpdateZScores
void UpdateZScores()
{
// Helpers
const auto fnGetIP = [](const Pitcher& pitcher) { return pitcher.IP;   };
const auto fnGetSO = [](const Pitcher& pitcher) { return pitcher.SO;   };
const auto fnGetW = [](const Pitcher& pitcher) { return pitcher.W;    };
const auto fnGetSV = [](const Pitcher& pitcher) { return pitcher.SV;   };
const auto fnGetERA = [](const Pitcher& pitcher) { return pitcher.ERA;  };
const auto fnGetWHIP = [](const Pitcher& pitcher) { return pitcher.WHIP; };

// Averages
const float avgIP = average(fnGetIP, m_vecPitchers);
const float avgSO = average(fnGetSO, m_vecPitchers);
const float avgW = average(fnGetW, m_vecPitchers);
const float avgSV = average(fnGetSV, m_vecPitchers);
const float avgERA = average(fnGetERA, m_vecPitchers);
const float avgWHIP = average(fnGetWHIP, m_vecPitchers);

// Standard devs
const float stdevIP = stdev(fnGetIP, avgIP, m_vecPitchers);
const float stdevSO = stdev(fnGetSO, avgSO, m_vecPitchers);
const float stdevW = stdev(fnGetW, avgW, m_vecPitchers);
const float stdevSV = stdev(fnGetSV, avgSV, m_vecPitchers);
const float stdevERA = stdev(fnGetERA, avgERA, m_vecPitchers);
const float stdevWHIP = stdev(fnGetWHIP, avgWHIP, m_vecPitchers);


}

// zscore
std::for_each(std::begin(m_vecPitchers), std::end(m_vecPitchers), [&](Pitcher& pitcher) {

    // per-state zscores
    pitcher.zSO = score(pitcher.SO, avgSO, stdevSO);
    pitcher.zW = score(pitcher.W, avgW, stdevW);
    pitcher.zSV = score(pitcher.SV, avgSV, stdevSV);
    pitcher.zERA = -(pitcher.IP * score(pitcher.ERA, avgERA, stdevERA) - stdevIP) / avgIP;
    pitcher.zWHIP = -(pitcher.IP * score(pitcher.WHIP, avgWHIP, stdevWHIP) - stdevIP) / avgIP;

    // zscore summation
    pitcher.zScore = pitcher.zSO + pitcher.zW + pitcher.zSV + pitcher.zERA + pitcher.zWHIP;
});

// Sort by zScore
std::sort(m_vecPitchers.begin(), m_vecPitchers.end(), [](const Pitcher& lhs, const Pitcher& rhs) {
    return lhs.zScore > rhs.zScore;
});

// Get the "replacement player"
float zRplacement = m_vecPitchers[Settings::TotalPitchersRostered].zScore;

// Scale all players based off the replacement player
std::for_each(std::begin(m_vecPitchers), std::end(m_vecPitchers), [&](Pitcher& pitcher) {
    pitcher.zScore -= zRplacement;
});

// Sum all positive zScores
float sumPositiveZScores = 0;
std::for_each(std::begin(m_vecPitchers), std::end(m_vecPitchers), [&](Pitcher& pitcher) {
    if (pitcher.zScore > 0) {
        sumPositiveZScores += pitcher.zScore;
    }
});

// Apply cost ratio
static const float costRatio = (Settings::PitcherMoney() / 10.f) * (Settings::TotalPitchersRostered / sumPositiveZScores);
std::for_each(std::begin(m_vecPitchers), std::end(m_vecPitchers), [&](Pitcher& pitcher) {
    pitcher.cost = pitcher.zScore * costRatio;
});

*/

class TableFilterAction : public QAction
{
public:
    
    TableFilterAction(QWidget* parent) 
        : QAction(parent)
    {
    }
    
    void setViews(const std::vector<QTableView*>& views)
    {
        for (auto& view : views) {
            connect(this, &QAction::triggered, view, &QTableView::resizeColumnsToContents);
        }
    }
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
        hitterSortFilterProxyModel->setFilterRole(HitterTableModel::RawDataRole);
        QTableView* hitterTableView = MakeTableView(hitterSortFilterProxyModel, 0);

        // pitcher table
        PitcherTableModel* pitcherTableModel = new PitcherTableModel("2015_pitchers.csv", appearances, this);
        PitcherSortFilterProxyModel* pitcherSortFilterProxyModel = new PitcherSortFilterProxyModel();
        pitcherSortFilterProxyModel->setSourceModel(pitcherTableModel);
        pitcherSortFilterProxyModel->setSortRole(PitcherTableModel::RawDataRole);
        QTableView* pitcherTableView = MakeTableView(pitcherSortFilterProxyModel, 0);

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

        // main toolbar
        QToolBar* toolbar = new QToolBar("Toolbar");
        toolbar->addWidget(new QLabel(" Leagues: ", this));
        toolbar->addActions(QList<QAction*>{filterAL, filterNL, filterFA});
        toolbar->addSeparator();
        toolbar->addWidget(new QLabel(" Positions: ", this));
        toolbar->addActions(QList<QAction*>{filterStarter, filterRelief});
        toolbar->addActions(QList<QAction*>{filterC, filter1B, filter2B, filterSS, filter3B, filterOF, filterDH});
        toolbar->setFloatable(false);
        toolbar->setMovable(false);
        QMainWindow::addToolBar(toolbar);

        // Helper to adjust filters
        auto ToggleFilterGroups = [=](int index)
        {
            switch (index)
            {
            case Tabs::Hitters:
                pitchingFilters->setVisible(false);
                hittingFilters->setVisible(true);
                break;
            case Tabs::Pitchers:
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
        model->sort(sortColumn);
        return tableView;
    }

    const QString m_style =
        R"""(
        QTableView, QHeaderView, QToolTip {
            font-family: "Consolas";
            font-size: 11px;
        })""";
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyle(QStyleFactory::create("Fusion"));

    // QPalette darkPalette;
    // darkPalette.setColor(QPalette::Window,QColor(53,53,53));
    // darkPalette.setColor(QPalette::WindowText,Qt::white);
    // darkPalette.setColor(QPalette::Base,QColor(25,25,25));
    // darkPalette.setColor(QPalette::AlternateBase,QColor(53,53,53));
    // darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
    // darkPalette.setColor(QPalette::ToolTipText,Qt::white);
    // darkPalette.setColor(QPalette::Text,Qt::white);
    // darkPalette.setColor(QPalette::Button,QColor(53,53,53));
    // darkPalette.setColor(QPalette::ButtonText,Qt::white);
    // darkPalette.setColor(QPalette::BrightText,Qt::red);
    // darkPalette.setColor(QPalette::Link,QColor(42,130,218));
    // darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
    // darkPalette.setColor(QPalette::HighlightedText,Qt::black);

    // app.setPalette(darkPalette);
    // app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

    MainWindow mainWin;
    mainWin.resize(1000, 800);
    mainWin.show();

    return app.exec();
}
