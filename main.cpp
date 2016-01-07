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

// TODO: move to settings
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

//------------------------------------------------------------------------------
// SummaryItemModel
//------------------------------------------------------------------------------

class SummaryTableModel : public QAbstractTableModel
{

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

        RANK_BA,
        RANK_R,
        RANK_HR,
        RANK_RBI,
        RANK_SB,

        ERA,
        WHIP,
        W,
        K,
        S,

        RANK_ERA,
        RANK_WHIP,
        RANK_W,
        RANK_K,
        RANK_S,
        RANK_SUM,

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
            return QString("%1 (%2)").arg(player.AB == 0 ? "--" : QString::number(player.AVG, 'f', 3)).arg(player.rankAVG);
        case RankRows::R:
            return QString("%1 (%2)").arg(player.R).arg(player.rankR);
        case RankRows::HR:
            return QString("%1 (%2)").arg(player.HR).arg(player.rankHR);
        case RankRows::RBI:
            return QString("%1 (%2)").arg(player.RBI).arg(player.rankRBI);
        case RankRows::SB:
            return QString("%1 (%2)").arg(player.SB).arg(player.rankSB);
        case RankRows::ERA:
            return QString("%1 (%2)").arg(player.IP == 0 ? "--" : QString::number(player.ERA, 'f', 3)).arg(player.rankERA);
        case RankRows::WHIP:
            return QString("%1 (%2)").arg(player.IP == 0 ? "--" : QString::number(player.WHIP, 'f', 3)).arg(player.rankWHIP);
        case RankRows::W:
            return QString("%1 (%2)").arg(player.W).arg(player.rankW);
        case RankRows::K:
            return QString("%1 (%2)").arg(player.K).arg(player.rankK);
        case RankRows::S:
            return QString("%1 (%2)").arg(player.S).arg(player.rankS);
        case RankRows::SUM:
            return QString("%1 (%2)").arg(player.SUM).arg("!");
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
            case RankRows::RANK_BA:
                return "AVG";
            case RankRows::R:
            case RankRows::RANK_R:
                return "R";
                return "AVG";
            case RankRows::HR:
            case RankRows::RANK_HR:
                return "HR";
            case RankRows::RBI:
            case RankRows::RANK_RBI:
                return "RBI";
            case RankRows::SB:
            case RankRows::RANK_SB:
                return "SB";
            case RankRows::ERA:
            case RankRows::RANK_ERA:
                return "ERA";
            case RankRows::WHIP:
            case RankRows::RANK_WHIP:
                return "WHIP";
            case RankRows::W:
            case RankRows::RANK_W:
                return "W";
            case RankRows::K:
            case RankRows::RANK_K:
                return "K";
            case RankRows::S:
            case RankRows::RANK_S:
                return "S";
            case RankRows::SUM:         // TODO: sigma sign
                return "SUM";
            default:
                break;
            }
        }

        return QVariant();
    }

public slots:

    void OnDrafted(const DraftDialog::Results& results, const QModelIndex& index, QAbstractItemModel* model)
    {
        auto GetValue = [=](uint32_t column)
        {
            QModelIndex indexHR = model->index(index.row(), column);
            return model->data(indexHR, PlayerTableModel::RawDataRole).toUInt();
        };

        auto DoGetRankingValue = [=](const auto& fnGet, const auto& fnSet)
        {
            std::vector<uint32_t> ranks(NUM_OWNERS);
            std::size_t n(0);
            std::generate(std::begin(ranks), std::end(ranks), [&] { return n++; });
            std::sort(std::begin(ranks), std::end(ranks), [&](uint32_t iLHS, uint32_t iRHS) {
                return fnGet(iLHS) < fnGet(iRHS);
            });

            for (uint32_t i = 0; i < NUM_OWNERS; i++) {
                fnSet(ranks[i], i + 1);
            }
        };

        #define GetRankingValue(STAT)                                                               \
            DoGetRankingValue(                                                                      \
                [&](uint32_t i) { return m_vecOwnerPoints[i].STAT; },                               \
                [&](uint32_t i, uint32_t rank) { return m_vecOwnerPoints[i].rank##STAT = rank; });

        // This owner
        SummaryTableModel::OwnerPoints& owner = m_vecOwnerPoints[results.ownerId];

        // Update values (hitting)
        owner.AB  += GetValue(PlayerTableModel::COLUMN_AB);
        owner.H   += GetValue(PlayerTableModel::COLUMN_H);
        owner.R   += GetValue(PlayerTableModel::COLUMN_R);
        owner.HR  += GetValue(PlayerTableModel::COLUMN_HR);
        owner.RBI += GetValue(PlayerTableModel::COLUMN_RBI);
        owner.SB  += GetValue(PlayerTableModel::COLUMN_SB);
        owner.AVG = owner.AB > 0 ? (owner.H / float(owner.AB)) : 0;

        // Update values (pitching)
        owner.IP  += GetValue(PlayerTableModel::COLUMN_IP);
        owner.HA  += GetValue(PlayerTableModel::COLUMN_HA);
        owner.BB  += GetValue(PlayerTableModel::COLUMN_BB);
        owner.ER  += GetValue(PlayerTableModel::COLUMN_ER);
        owner.W   += GetValue(PlayerTableModel::COLUMN_W);
        owner.K   += GetValue(PlayerTableModel::COLUMN_SO);
        owner.S   += GetValue(PlayerTableModel::COLUMN_SV);
        owner.ERA = owner.IP > 0 ? ((9.f*owner.ER) / owner.IP) : 0;
        owner.WHIP = owner.IP > 0 ? ((owner.HA + owner.BB) / owner.IP) : 0;

        // Update rankings (hitting)
        GetRankingValue(AVG);
        GetRankingValue(R);
        GetRankingValue(HR);
        GetRankingValue(RBI);
        GetRankingValue(SB);

        // Update rankings (pitching)
        GetRankingValue(W);
        GetRankingValue(K);
        GetRankingValue(S);
        GetRankingValue(ERA);
        GetRankingValue(WHIP);

        // Update sum
        owner.SUM = owner.rankAVG + owner.rankR + owner.rankHR + owner.rankRBI + owner.rankSB +
            owner.rankW + owner.rankK + owner.rankS + owner.rankERA + owner.rankWHIP;
    }

private:

    struct OwnerPoints
    {
        // Hitting (values)
        uint32_t AB = 0;
        uint32_t H = 0;
        float AVG = 0;
        uint32_t R = 0;
        uint32_t HR = 0;
        uint32_t RBI = 0;
        uint32_t SB = 0;

        // Hitting (ranks)
        uint32_t rankAVG = 0;
        uint32_t rankR = 0;
        uint32_t rankHR = 0;
        uint32_t rankRBI = 0;
        uint32_t rankSB = 0;

        // Pitching
        float IP = 0;
        uint32_t HA = 0;
        uint32_t BB = 0;
        uint32_t ER = 0;
        float ERA = 0;
        float WHIP = 0;
        uint32_t W = 0;
        uint32_t K = 0;
        uint32_t S = 0;

        // Pitching (ranks)
        uint32_t rankERA = 0;
        uint32_t rankWHIP = 0;
        uint32_t rankW = 0;
        uint32_t rankK = 0;
        uint32_t rankS = 0;

        // Summary
        uint32_t SUM;
    };

    std::vector<OwnerPoints> m_vecOwnerPoints;
};

//----------------------------------------------------------------------
// SummaryWidget
//----------------------------------------------------------------------

class SummaryWidget : public QWidget
{

public:

    SummaryWidget(SummaryTableModel* model, QWidget* parent)
        : QWidget(parent)
        , m_sumProxyModel(new SumProxyModel(parent))
        , m_sumTableView(new QTableView)
        , m_layout(new QVBoxLayout)
    {
        // proxy models
        m_sumProxyModel->setSourceModel(model);

        // sum table view
        m_sumTableView->setModel(m_sumProxyModel);
        m_sumTableView->verticalHeader()->hide();
        m_sumTableView->setStyleSheet(TableStyle());
        m_sumTableView->setAlternatingRowColors(true);
        m_sumTableView->verticalHeader()->setDefaultSectionSize(15);
        m_sumTableView->setCornerButtonEnabled(true);
        
        // 100% width
        for (int i = 0; i < m_sumTableView->horizontalHeader()->count(); ++i) {
            m_sumTableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }

        // main layout
        setLayout(m_layout);
        m_layout->addWidget(m_sumTableView);
        m_layout->addStretch();
    }

private:

    class SumProxyModel : public QSortFilterProxyModel
    {
    public:
        SumProxyModel(QWidget* parent)
            : QSortFilterProxyModel(parent)
        {
        }

        bool filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const override
        {
            switch (sourceColumn)
            {
            case SummaryTableModel::TEAM:
            case SummaryTableModel::BA:
            case SummaryTableModel::R:
            case SummaryTableModel::HR:
            case SummaryTableModel::RBI:
            case SummaryTableModel::SB:
            case SummaryTableModel::ERA:
            case SummaryTableModel::WHIP:
            case SummaryTableModel::W:
            case SummaryTableModel::K:
            case SummaryTableModel::S:
            case SummaryTableModel::SUM:
                return true;
            default:
                return false;
            }
        }
    };

    QVBoxLayout* m_layout;
    SumProxyModel* m_sumProxyModel;
    QTableView* m_sumTableView;
};

//------------------------------------------------------------------------------
// OwnerItemModel
//------------------------------------------------------------------------------

class OwnerItemModel : public QAbstractTableModel
{
public:

    enum Columns
    {
        COLUMN_DRAFT_POSITION,
        COLUMN_PAID,
        COLUMN_NAME,
        COUNT,
    };

    OwnerItemModel(uint32_t ownerId, QWidget* parent) 
        : QAbstractTableModel(parent)
        , m_ownerId(ownerId)
    {
        // Valid positions
        QStringList vecDraftPositions = {
            "C", "C", "1B", "2B", "SS", "3B", "MI", "CI", "OF", "OF", "OF", "OF", "OF", "U",
            "P", "P", "P", "P", "P", "P", "P", "P", "P", "P",
        };

        // Loop 'em
        for (const QString& draftPosition : vecDraftPositions) {
            m_draftedPlayers.push_back(std::make_pair(draftPosition, nullptr));
        }
    }

    virtual int rowCount(const QModelIndex& index) const override
    {
        return m_draftedPlayers.size();
    }

    virtual int columnCount(const QModelIndex& index) const override
    {
        return Columns::COUNT;
    }

    virtual QVariant data(const QModelIndex& index, int role) const override
    {
        if (role != Qt::DisplayRole) {
            return QVariant();
        }

        const QString& position = m_draftedPlayers[index.row()].first;
        const std::shared_ptr<OwnedPlayer>& spPlayer = m_draftedPlayers[index.row()].second;

        switch (index.column())
        {
        case COLUMN_DRAFT_POSITION:
            return position;
        case COLUMN_PAID:
            return spPlayer ? QString("$1").arg(spPlayer->cost) : QVariant("--");
        case COLUMN_NAME:
            return spPlayer ? QVariant(spPlayer->name) : QVariant("--");
        default:
            break;
        }

        return QVariant();

    }

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {

            switch (section)
            {
            case COLUMN_DRAFT_POSITION:
                return "Pos.";
            case COLUMN_PAID:
                return "Paid";
            case COLUMN_NAME:
                return "Name";
            }
        }

        return QVariant();
    }


public slots:

    void OnDrafted(const DraftDialog::Results& results, const QModelIndex& index, QAbstractItemModel* model)
    {
        // Ignore other owners
        if (results.ownerId != m_ownerId) {
            return;
        }

        // Find owner name
        QString name = model->data(model->index(index.row(), PlayerTableModel::COLUMN_NAME), Qt::DisplayRole).toString();

        // Find the first opening in this position
        auto itr = std::find_if(m_draftedPlayers.begin(), m_draftedPlayers.end(), [&](const PlayerPair& pp) {
            return (pp.first == results.position && !pp.second);
        });

        // Insert this play in the opening
        if (itr != m_draftedPlayers.end()) {
            itr->second = std::make_shared<OwnedPlayer>(name, results.cost);
            return;
        }

        // No opening at this position so create a new row; we can edit it later
        beginInsertRows(QModelIndex(), m_draftedPlayers.size(), m_draftedPlayers.size());
        m_draftedPlayers.push_back(std::make_pair("??", std::make_shared<OwnedPlayer>(name, results.cost)));
        endInsertRows();
    }

private:

    struct OwnedPlayer
    {
        OwnedPlayer(QString name, uint32_t cost)
            : name(name)
            , cost(cost)
        {}

        QString name;
        uint32_t cost;
    };

    uint32_t m_ownerId;

    using PlayerPair = std::pair<QString, std::shared_ptr<OwnedPlayer>>;
    std::vector<PlayerPair> m_draftedPlayers;
};

//------------------------------------------------------------------------------
// MainWindow
//------------------------------------------------------------------------------

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

        // Owner widget
        QHBoxLayout* ownersLayout = new QHBoxLayout(this);

        // Loop owners
        for (int ownerId = 1; ownerId <= NUM_OWNERS; ownerId++) {

            // V-Layout per owner
            QVBoxLayout* perOwnerLayout = new QVBoxLayout(this);

            // Item model for this owner
            OwnerItemModel* ownerItemModel = new OwnerItemModel(ownerId, this);
            connect(draftDelegate, &DraftDelegate::Drafted, ownerItemModel, &OwnerItemModel::OnDrafted);

            // Owner name label
            QLabel* ownerLabel = new QLabel(QString("Owner #%1").arg(ownerId), this);
            ownerLabel->setAlignment(Qt::AlignCenter);
            perOwnerLayout->addWidget(ownerLabel);

            // Table view
            QTableView* ownerTableView = MakeTableView(ownerItemModel, false);
            ownerTableView->setFixedWidth(225);
            perOwnerLayout->addWidget(ownerTableView);

            // Summary labels
            QLabel* summary = new QLabel( "Total: $0", this);
            perOwnerLayout->addWidget(summary);
            
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
            virtual QSize sizeHint() const override { return QSize(220, 500); }
        };

        // Owner scroll area
        OwnerScrollArea* ownerScrollArea = new OwnerScrollArea(this);
        ownerScrollArea->setWidget(ownerWidget);
        ownerScrollArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        ownerScrollArea->setBackgroundRole(QPalette::Light);

        // Player scatter plot
        PlayerScatterPlotChart* chartView = new PlayerScatterPlotChart(playerTableModel, hitterSortFilterProxyModel, this);
        connect(hitterSortFilterProxyModel,  &QSortFilterProxyModel::layoutChanged, chartView, &PlayerScatterPlotChart::Update);
        connect(pitcherSortFilterProxyModel, &QSortFilterProxyModel::layoutChanged, chartView, &PlayerScatterPlotChart::Update);
        connect(playerTableModel, &QAbstractItemModel::dataChanged, chartView, &PlayerScatterPlotChart::Update);

        // Summary model
        SummaryTableModel* summaryModel = new SummaryTableModel(NUM_OWNERS, this);

        // Summary view
        SummaryWidget* summary = new SummaryWidget(summaryModel, this);

        // Bottom tabs
        enum BottomSectionTabs { Rosters, SummaryWidget, ChartVie, TEST };
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

        // Connect drafted model

        // Connect summary model
        connect(draftDelegate, &DraftDelegate::Drafted, summaryModel, &SummaryTableModel::OnDrafted);

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

