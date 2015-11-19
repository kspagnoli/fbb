#include <QApplication>
#include <QLabel>
#include <QAbstractTableModel>
#include <QTableView>
#include <QMainWindow>
#include <QDockWidget>
#include <QStyleFactory>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QSplitter>
#include <QTableWidget>
#include <QAbstractItemModel>

#include <memory>
#include <cstdint>
#include <fstream> 
#include <string>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <bitset>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include "Hitter.h"
#include "HitterTableModel.h"
#include "HitterSortFilterProxyModel.h"

#include "Pitcher.h"
#include "PitcherTableModel.h"
#include "PitcherSortFilterProxyModel.h"

enum class Leauge
{
    Unknown,
    AL,
    NL,
};

enum class Division
{
    Unknown,
    ALEast,
    ALCentral,
    ALWest,
    NLEast,
    NLCentral,
    NLWest,
};

struct TeamGroup
{
    Leauge leauge;
    Division division;
};

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

static TeamGroup LookupTeamGroup(const std::string& teamName)
{
    static std::unordered_map<std::string, TeamGroup> s_LUT = {

        {"Orioles",{Leauge::AL,Division::ALEast}},
        {"Red Sox",{Leauge::AL,Division::ALEast}},
        {"Yankees",{Leauge::AL,Division::ALEast}},
        {"Rays",{Leauge::AL,Division::ALEast}},
        {"Blue Jays",{Leauge::AL,Division::ALEast}},
        {"Braves",{Leauge::NL,Division::NLEast}},
        {"Marlins",{Leauge::NL,Division::NLEast}},
        {"Mets",{Leauge::NL,Division::NLEast}},
        {"Phillies",{Leauge::NL,Division::NLEast}},
        {"Nationals",{Leauge::NL,Division::NLEast}},
        {"White Sox",{Leauge::AL,Division::ALCentral}},
        {"Indians",{Leauge::AL,Division::ALCentral}},
        {"Tigers",{Leauge::AL,Division::ALCentral}},
        {"Royals",{Leauge::AL,Division::ALCentral}},
        {"Twins",{Leauge::AL,Division::ALCentral}},
        {"Cubs",{Leauge::NL,Division::NLCentral}},
        {"Reds",{Leauge::NL,Division::NLCentral}},
        {"Brewers",{Leauge::NL,Division::NLCentral}},
        {"Pirates",{Leauge::NL,Division::NLCentral}},
        {"Cardinals",{Leauge::NL,Division::NLCentral}},
        {"Astros",{Leauge::AL,Division::ALWest}},
        {"Angels",{Leauge::AL,Division::ALWest}},
        {"Athletics",{Leauge::AL,Division::ALWest}},
        {"Mariners",{Leauge::AL,Division::ALWest}},
        {"Rangers",{Leauge::AL,Division::ALWest}},
        {"Diamondbacks",{Leauge::NL,Division::NLWest}},
        {"Rockies",{Leauge::NL,Division::NLWest}},
        {"Dodgers",{Leauge::NL,Division::NLWest}},
        {"Padres",{Leauge::NL,Division::NLWest}},
        {"Giants",{Leauge::NL,Division::NLWest}},

    };

    return s_LUT[teamName];
}

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


//
class MainWindow : public QMainWindow
{

public:

    MainWindow()
    {
        QWidget* central = new QWidget();

        // Hitter table
        HitterTableModel* hitterTableModel = new HitterTableModel("batters.csv", this);
        HitterSortFilterProxyModel* hitterSortFilterProxyModel = new HitterSortFilterProxyModel();
        hitterSortFilterProxyModel->setSourceModel(hitterTableModel);
        QTableView* hitterTableView = MakeTableView(hitterSortFilterProxyModel, 0);

        // Pitcher table
        PitcherTableModel* pitcherTableModel = new PitcherTableModel("pitchers.csv", this);
        PitcherSortFilterProxyModel* pitcherSortFilterProxyModel = new PitcherSortFilterProxyModel();
        pitcherSortFilterProxyModel->setSourceModel(pitcherTableModel);
        hitterSortFilterProxyModel->setSortRole(PitcherTableModel::RawDataRole);
        QTableView* pitcherTableView = MakeTableView(pitcherSortFilterProxyModel, 0);

        // layout
        QVBoxLayout* vBoxLayout = new QVBoxLayout();

        // hitter/pitcher tab View
        QTabWidget* tabs = new QTabWidget(this);
        tabs->addTab(hitterTableView, "Hitters");
        tabs->addTab(pitcherTableView, "Pitchers");
        vBoxLayout->addWidget(tabs);

        // filter action
        QAction* filterAvailableAction = new QAction(this);
        filterAvailableAction->setText(tr("Filter Available"));
        filterAvailableAction->setCheckable(true);
        connect(filterAvailableAction, &QAction::triggered, hitterSortFilterProxyModel, &HitterSortFilterProxyModel::OnFilterAvailable);
        connect(filterAvailableAction, &QAction::triggered, pitcherSortFilterProxyModel, &PitcherSortFilterProxyModel::OnFilterAvailable);

        // main toolbar
        QToolBar* toolbar = new QToolBar("Toolbar");
        toolbar->addAction(filterAvailableAction);
        toolbar->setFloatable(false);
        toolbar->setMovable(false);
        QMainWindow::addToolBar(toolbar);

        // set as main window
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

    //
    QTableView* MakeTableView(QAbstractItemModel* model, int sortColumn)
    {
        QTableView* tableView = new QTableView();
        tableView->setModel(model);
        tableView->setSortingEnabled(true);
        tableView->verticalHeader()->hide();
        tableView->setStyleSheet(m_style);
        tableView->resizeColumnsToContents();
        tableView->setAlternatingRowColors(true);
        tableView->verticalHeader()->setDefaultSectionSize(15);
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
