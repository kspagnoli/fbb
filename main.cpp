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
#include <QFileDialog>

#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "DraftDialog.h"
#include "DraftSettings.h"
#include "PlayerTableModel.h"
#include "PlayerSortFilterProxyModel.h"
#include "SelectedPlayer.h"
#include "PlayerAppearances.h"
#include "DraftDelegate.h"
#include "OwnerSortFilterProxyModel.h"
#include "PlayerScatterPlotChart.h"

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

    SummaryTableModel(const std::vector<OwnerSortFilterProxyModel*>& vecOwnerSortFilterProxyModel, QWidget* parent)
        : QAbstractTableModel(parent)
        , m_vecOwnerSortFilterProxyModels(vecOwnerSortFilterProxyModel)
        , m_vecOwnerPoints(vecOwnerSortFilterProxyModel.size())
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
        return int(m_vecOwnerSortFilterProxyModels.size());
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

        // const OwnerPoints& player = m_vecOwnerPoints.at(index.row());

        auto ownerSortFilterProxyModel = m_vecOwnerSortFilterProxyModels[index.row()];
        auto ownerPointer = m_vecOwnerPoints[index.row()];
        
        switch (index.column())
        {
            case RankRows::TEAM:
            {
                return DraftSettings::OwnerAbbreviation(index.row() + 1);
            }
            case RankRows::BA:
            {
                auto BA = ownerSortFilterProxyModel->AVG();
                QString strBA = (BA == 0.f ? QString("--") : QString::number(BA, 'f', 3));
                return QString("%1 (%2)").arg(strBA).arg(ownerPointer.rankAVG);
            }
            case RankRows::R:
            {
                auto R = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_R);
                return QString("%1 (%2)").arg(R).arg(ownerPointer.rankR);
            }
            case RankRows::HR:
            {
                auto HR = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_HR);
                return QString("%1 (%2)").arg(HR).arg(ownerPointer.rankHR);
            }
            case RankRows::RBI:
            {
                auto RBI = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_RBI);
                return QString("%1 (%2)").arg(RBI).arg(ownerPointer.rankRBI);
            }
            case RankRows::SB:
            {
                auto SB = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_SB);
                return QString("%1 (%2)").arg(SB).arg(ownerPointer.rankSB);
            }
            case RankRows::ERA:
            {
                auto ERA = ownerSortFilterProxyModel->ERA();
                QString strERA = (ERA == 0.f ? QString("--") : QString::number(ERA, 'f', 3));
                return QString("%1 (%2)").arg(strERA).arg(ownerPointer.rankERA);
            }
            case RankRows::WHIP:
            {
                auto WHIP = ownerSortFilterProxyModel->WHIP();
                QString strWHIP = (WHIP == 0.f ? QString("--") : QString::number(WHIP, 'f', 3));
                return QString("%1 (%2)").arg(strWHIP).arg(ownerPointer.rankWHIP);
            }
            case RankRows::W:
            {
                auto W = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_W);
                return QString("%1 (%2)").arg(W).arg(ownerPointer.rankW);
            }
            case RankRows::K:
            {
                auto SO = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_SO);
                return QString("%1 (%2)").arg(SO).arg(ownerPointer.rankK);
            }
            case RankRows::S:
            {
                auto SV = ownerSortFilterProxyModel->Sum(PlayerTableModel::COLUMN_SV);
                return QString("%1 (%2)").arg(SV).arg(ownerPointer.rankSV);
            }
            case RankRows::SUM:
            {
                return QString("%1 (%2)").arg(ownerPointer.SUM).arg(ownerPointer.rankSUM);
            }
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
            case RankRows::SUM:
                return "SUM";
            default:
                break;
            }
        }

        return QVariant();
    }

public slots:

    void OnDraftedEnd()
    {
        auto RankValues = [=](auto FnGet, auto FnSet)
        {
            std::vector<uint32_t> ranks(DraftSettings::OwnerCount());
            std::vector<float> values(DraftSettings::OwnerCount());
            std::size_t n(0);
            std::generate(std::begin(ranks), std::end(ranks), [&] { return n++; });

            for (size_t i = 0; i < m_vecOwnerSortFilterProxyModels.size(); i++) {
                values[i] = FnGet(i);
            }

            std::sort(std::begin(ranks), std::end(ranks), [&](uint32_t iLHS, uint32_t iRHS) {
                return values[iLHS] < values[iRHS];
            });

            for (size_t i = 0; i < m_vecOwnerPoints.size(); i++) {
                FnSet(ranks[i], i+1);
            }
        };

        // AVG
        RankValues(
            [=](size_t i)          { return m_vecOwnerSortFilterProxyModels[i]->AVG(); },
            [=](size_t i, float x) { return m_vecOwnerPoints[i].rankAVG = x; }
        );

        // R
        RankValues(
            [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_R); },
            [=](size_t i, float x) { return m_vecOwnerPoints[i].rankR = x; }
        );

        // HR
        RankValues(
            [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_HR); },
            [=](size_t i, float x) { return m_vecOwnerPoints[i].rankHR = x; }
        );

        // RBI
        RankValues(
            [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_RBI); },
            [=](size_t i, float x) { return m_vecOwnerPoints[i].rankRBI = x; }
        );

        // SB
        RankValues(
            [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_SB); },
            [=](size_t i, float x) { return m_vecOwnerPoints[i].rankSB = x; }
        );

        // ERA
        RankValues(
            [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->ERA(); },
            [=](size_t i, float x) { return m_vecOwnerPoints[i].rankERA = x; }
        );

        // WHIP
        RankValues(
            [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->WHIP(); },
            [=](size_t i, float x) { return m_vecOwnerPoints[i].rankWHIP = x; }
        );

        // W
        RankValues(
            [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_W); },
            [=](size_t i, float x) { return m_vecOwnerPoints[i].rankW = x; }
        );

        // K
        RankValues(
            [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_SO); },
            [=](size_t i, float x) { return m_vecOwnerPoints[i].rankK = x; }
        );

        // SV
        RankValues(
            [=](size_t i) { return m_vecOwnerSortFilterProxyModels[i]->Sum(PlayerTableModel::COLUMN_SV); },
            [=](size_t i, float x) { return m_vecOwnerPoints[i].rankSV = x; }
        );

        // Invert "lower-is-better" stats
        for (auto& owner : m_vecOwnerPoints) {
            owner.rankERA = m_vecOwnerPoints.size() - owner.rankERA + 1;
            owner.rankWHIP = m_vecOwnerPoints.size() - owner.rankWHIP + 1;
        }

        // Calculate sum
        for (auto& owner : m_vecOwnerPoints) {
            owner.SUM = owner.rankAVG + owner.rankR + owner.rankHR + owner.rankRBI + owner.rankSB + owner.rankERA + owner.rankWHIP + owner.rankW + owner.rankK + owner.rankSV;
        }
        
        // SV
        RankValues(
            [=](size_t i) { return m_vecOwnerPoints[i].SUM; },
            [=](size_t i, float x) { return m_vecOwnerPoints[i].rankSUM = x; }
        );

    }

private:

    struct OwnerPoints
    {
        // Hitting (ranks)
        uint32_t rankAVG = 0;
        uint32_t rankR = 0;
        uint32_t rankHR = 0;
        uint32_t rankRBI = 0;
        uint32_t rankSB = 0;

        // Pitching (ranks)
        uint32_t rankERA = 0;
        uint32_t rankWHIP = 0;
        uint32_t rankW = 0;
        uint32_t rankK = 0;
        uint32_t rankSV = 0;

        // Summary
        uint32_t SUM = 0;
        uint32_t rankSUM = 0;
    };

    std::vector<OwnerSortFilterProxyModel*> m_vecOwnerSortFilterProxyModels;
    std::vector<OwnerPoints> m_vecOwnerPoints;
};

//----------------------------------------------------------------------
// SummaryWidget
//----------------------------------------------------------------------

class SummaryWidget : public QWidget
{

public:

    SummaryWidget(const std::vector<OwnerSortFilterProxyModel*>& vecOwnerSortFilterProxyModel, QWidget* parent)
        : QWidget(parent)
        , m_sumTableView(new QTableView)
        , m_layout(new QVBoxLayout)
    {

        m_summaryTableModel = new SummaryTableModel(vecOwnerSortFilterProxyModel, parent);

        // sum table view
        m_sumTableView->setModel(m_summaryTableModel);
        m_sumTableView->verticalHeader()->hide();
        m_sumTableView->setStyleSheet(TableStyle());
        m_sumTableView->setAlternatingRowColors(true);
        m_sumTableView->verticalHeader()->setDefaultSectionSize(15);
        m_sumTableView->setCornerButtonEnabled(true);
        m_sumTableView->setSortingEnabled(true);
        m_sumTableView->sortByColumn(SummaryTableModel::TEAM);
        
        // 100% width
        for (int i = 0; i < m_sumTableView->horizontalHeader()->count(); ++i) {
            m_sumTableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }

        // main layout
        setLayout(m_layout);
        m_layout->addWidget(m_sumTableView);
        m_layout->addStretch();
    }

public slots:

    void OnDraftedEnd()
    {
        // Forward to table
        m_summaryTableModel->OnDraftedEnd();
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
    SummaryTableModel* m_summaryTableModel;
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
            return spPlayer ? QString("$%1").arg(spPlayer->cost) : QVariant("--");
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
        if ((results.ownerId != m_ownerId) && (results.previousOwnerId != m_ownerId)) {
            return;
        }

        // Find player id & name
        uint32_t playerId = model->data(model->index(index.row(), PlayerTableModel::COLUMN_ID), Qt::DisplayRole).toUInt();
        QString name = model->data(model->index(index.row(), PlayerTableModel::COLUMN_NAME), Qt::DisplayRole).toString();
        
        // If removing a player...
        if (results.previousOwnerId) {

            // Find this player id
            auto itr = std::find_if(m_draftedPlayers.begin(), m_draftedPlayers.end(), [&](const PlayerPair& pp) {
                return pp.second && (pp.second->id == playerId);
            });

            // Remove this player
            if (itr != m_draftedPlayers.end()) {
                auto pos = itr->first;
                itr->second.reset();
                if (pos == "??") {
                    beginRemoveRows(QModelIndex(), m_draftedPlayers.size() - 1, m_draftedPlayers.size() - 1);
                    m_draftedPlayers.erase(itr);
                    endRemoveRows();
                }
            }
        }

        // If adding a player...
        if (results.ownerId) {

            // Find the first opening in this position
            auto itr = std::find_if(m_draftedPlayers.begin(), m_draftedPlayers.end(), [&](const PlayerPair& pp) {
                QString pos = PositionToString(results.position);
                return (pp.first == pos && !pp.second);
            });

            // Insert this player in the opening
            if (itr != m_draftedPlayers.end()) {
                itr->second = std::make_shared<OwnedPlayer>(name, results.cost, playerId);
                return;
            }

            // No opening at this position so create a new row; we can edit it later
            beginInsertRows(QModelIndex(), m_draftedPlayers.size(), m_draftedPlayers.size());
            m_draftedPlayers.push_back(std::make_pair("??", std::make_shared<OwnedPlayer>(name, results.cost, playerId)));
            endInsertRows();
        }
    }

private:

    struct OwnedPlayer
    {
        OwnedPlayer(QString name, uint32_t cost, uint32_t id)
            : name(name)
            , cost(cost)
            , id(id)
        {}

        QString name;
        uint32_t cost;
        uint32_t id;
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

        // Player table model
        PlayerTableModel* playerTableModel = new PlayerTableModel(this);
        playerTableModel->LoadHittingProjections("2015_hitters.csv", appearances);
        playerTableModel->LoadPitchingProjections("2015_pitchers.csv", appearances);

        // Draft delegate
        DraftDelegate* draftDelegate = new DraftDelegate(playerTableModel);

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
        QAction* filterCI = MakeHitterFilter("CI", "Filter Corner Infielders",  &PlayerSortFilterProxyModel::OnFilterCI);
        QAction* filterMI = MakeHitterFilter("MI", "Filter Middle Infielders",  &PlayerSortFilterProxyModel::OnFilterMI);
        QAction* filterDH = MakeHitterFilter("DH", "Filter Designated Hitters", &PlayerSortFilterProxyModel::OnFilterDH);
        QAction* filterU  = MakeHitterFilter("U",  "Filter Utility",            &PlayerSortFilterProxyModel::OnFilterU);

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
        auto HighlightPlayerInTable = [=](const QModelIndex& srcIdx)
        {
            // Lookup catergory
            auto catergoryIdx = srcIdx.model()->index(srcIdx.row(), PlayerTableModel::COLUMN_CATERGORY);
            auto catergory = srcIdx.model()->data(catergoryIdx).toUInt();

            // Change to tab
            hitterPitcherTabs->setCurrentIndex(CaterogyToTab(catergory));

            // Select row
            if (catergory == Player::Catergory::Hitter) {
                auto proxyModel = dynamic_cast<QSortFilterProxyModel*>(hitterTableView->model());
                auto proxyIdx = proxyModel->mapFromSource(srcIdx);
                hitterTableView->selectRow(proxyIdx.row());
                hitterTableView->setFocus();
            } else if (catergory == Player::Catergory::Pitcher) {
                auto proxyModel = dynamic_cast<QSortFilterProxyModel*>(pitcherTableView->model());
                auto proxyIdx = proxyModel->mapFromSource(srcIdx);
                pitcherTableView->selectRow(proxyIdx.row());
                pitcherTableView->setFocus();
            }
        };

        // Select the target 
        connect(completer, static_cast<void (QCompleter::*)(const QModelIndex&)>(&QCompleter::activated), [=](const QModelIndex& index) {

            // Get player index
            QAbstractProxyModel* proxyModel = dynamic_cast<QAbstractProxyModel*>(completer->completionModel());
            auto srcIdx = proxyModel->mapToSource(index);
            
            // Highlight this player
            HighlightPlayerInTable(srcIdx);
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
        toolbar->addActions(QList<QAction*>{filterC, filter1B, filter2B, filterSS, filter3B, filterOF, filterCI, filterMI, filterDH, filterU});
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
        ownersLayout->setSizeConstraint(QLayout::SetNoConstraint);

        // Owner models
        std::vector<OwnerSortFilterProxyModel*> vecOwnerSortFilterProxyModels;

        // Loop owners
        for (uint32_t ownerId = 1; ownerId <= DraftSettings::OwnerCount(); ownerId++) {

            // V-Layout per owner
            QVBoxLayout* perOwnerLayout = new QVBoxLayout(this);
            ownersLayout->addLayout(perOwnerLayout);
            perOwnerLayout->setSizeConstraint(QLayout::SetNoConstraint);

            // Proxy model for this owner
            OwnerSortFilterProxyModel* ownerSortFilterProxyModel = new OwnerSortFilterProxyModel(ownerId, playerTableModel, this);
            connect(playerTableModel, &PlayerTableModel::Drafted, ownerSortFilterProxyModel, &OwnerSortFilterProxyModel::OnDrafted);
            vecOwnerSortFilterProxyModels.push_back(ownerSortFilterProxyModel);

            // Owner name label
            QLabel* ownerLabel = new QLabel(DraftSettings::OwnerName(ownerId), this);
            ownerLabel->setAlignment(Qt::AlignCenter);
            perOwnerLayout->addWidget(ownerLabel);

            // Per-owner roster table view
            QTableView* ownerRosterTableView = MakeTableView(ownerSortFilterProxyModel, true, 0);
            ownerRosterTableView->setMinimumSize(200, 65);
            ownerRosterTableView->setMaximumSize(200, 4096);
            ownerRosterTableView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            perOwnerLayout->addWidget(ownerRosterTableView);

            QGridLayout* ownerSummaryGridLayout = new QGridLayout(this);
            ownerSummaryGridLayout->setSpacing(0);
            ownerSummaryGridLayout->addWidget(new QLabel("Budget: "),     0, 0);
            ownerSummaryGridLayout->addWidget(new QLabel("# Hitters: "),  1, 0);
            ownerSummaryGridLayout->addWidget(new QLabel("# Pitchers: "), 2, 0);
            ownerSummaryGridLayout->addWidget(new QLabel("Max Bid: "),    3, 0);

            QLabel* budgetLabel = new QLabel(this);
            QLabel* numHittersLabel = new QLabel(this);
            QLabel* numPitchersLabel = new QLabel(this);
            QLabel* maxBidLabel = new QLabel(this);

            // Helper
            auto UpdateLabels = [=]()
            {
                budgetLabel->setText(QString("$%1").arg(ownerSortFilterProxyModel->GetRemainingBudget()));
                numHittersLabel->setText(QString("%1 / %2").arg(ownerSortFilterProxyModel->Count(Player::Hitter)).arg(DraftSettings::HitterCount()));
                numPitchersLabel->setText(QString("%1 / %2").arg(ownerSortFilterProxyModel->Count(Player::Pitcher)).arg(DraftSettings::PitcherCount()));
                maxBidLabel->setText(QString("$%1").arg(ownerSortFilterProxyModel->GetMaxBid()));
            };

            // Update labels when a draft event happens
            connect(playerTableModel, &PlayerTableModel::Drafted, [=](const DraftDialog::Results& results, const QModelIndex& index, QAbstractItemModel* model){
                if (results.ownerId == ownerId) {
                    UpdateLabels();
                }
            });

            UpdateLabels();

            ownerSummaryGridLayout->addWidget(budgetLabel,      0, 1);
            ownerSummaryGridLayout->addWidget(numHittersLabel,  1, 1);
            ownerSummaryGridLayout->addWidget(numPitchersLabel, 2, 1);
            ownerSummaryGridLayout->addWidget(maxBidLabel,      3, 1);

            QSpacerItem* spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);

            ownerSummaryGridLayout->addItem(spacer, 0, 2);
            ownerSummaryGridLayout->addItem(spacer, 1, 2);
            ownerSummaryGridLayout->addItem(spacer, 2, 2);
            ownerSummaryGridLayout->addItem(spacer, 3, 2);
            perOwnerLayout->addLayout(ownerSummaryGridLayout);

            perOwnerLayout->addSpacerItem(spacer);
        }

        // Owner widget
        QWidget* scrollAreaWidgetContents = new QWidget(this);
        scrollAreaWidgetContents->setLayout(ownersLayout);
        scrollAreaWidgetContents->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        // Owner scroll area
        QScrollArea* ownerScrollArea = new QScrollArea(this);
        ownerScrollArea->setWidget(scrollAreaWidgetContents);
        ownerScrollArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        ownerScrollArea->setBackgroundRole(QPalette::Light);
        ownerScrollArea->setFrameShape(QFrame::NoFrame);
        ownerScrollArea->setWidgetResizable(true);

        // Player scatter plot
        PlayerScatterPlotChart* chartView = new PlayerScatterPlotChart(playerTableModel, hitterSortFilterProxyModel, this);
        connect(hitterSortFilterProxyModel,  &QSortFilterProxyModel::layoutChanged, chartView, &PlayerScatterPlotChart::Update);
        connect(pitcherSortFilterProxyModel, &QSortFilterProxyModel::layoutChanged, chartView, &PlayerScatterPlotChart::Update);
        connect(playerTableModel, &QAbstractItemModel::dataChanged, chartView, &PlayerScatterPlotChart::Update);

        // Summary view
        SummaryWidget* summary = new SummaryWidget(vecOwnerSortFilterProxyModels, this);

        // Bottom tabs
        enum BottomSectionTabs { Rosters, SummaryWidget, ChartView };
        QTabWidget* bottomTabs = new QTabWidget(this);
        topBottomSplitter->addWidget(bottomTabs);
        bottomTabs->insertTab(BottomSectionTabs::Rosters, ownerScrollArea, "Rosters");
        bottomTabs->insertTab(BottomSectionTabs::SummaryWidget, summary, "Summary");
        bottomTabs->insertTab(BottomSectionTabs::ChartView, chartView, "Scatter Chart");

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
        
        // Connect summary model
        connect(playerTableModel, &PlayerTableModel::DraftedEnd, summary, &SummaryWidget::OnDraftedEnd);

        //----------------------------------------------------------------------
        // Main
        //----------------------------------------------------------------------

        // Set as main window
        QMainWindow::setCentralWidget(topBottomSplitter);

        // Create main menu bar
        QMenuBar* mainMenuBar = new QMenuBar();
        QMainWindow::setMenuBar(mainMenuBar);
        
        // Main Menu > File menu
        QMenu* fileMenu = mainMenuBar->addMenu("&File");

        // File dialog helper
        auto GetFileDialog = [&](QFileDialog::AcceptMode mode) -> QFileDialog*
        {
            QFileDialog* dialog = new QFileDialog(this);
            dialog->setWindowModality(Qt::WindowModal);
            dialog->setAcceptMode(mode);
            dialog->setNameFilter("CSV files (*.csv)");
            return dialog;
        };

        // Main Menu > File menu > Save action
        QAction* saveResultsAction = new QAction("&Save Results...", this);
        connect(saveResultsAction, &QAction::triggered, [=](bool checked) {

            auto dialog = GetFileDialog(QFileDialog::AcceptSave);

            QStringList files;
            if (dialog->exec()) {
                files = dialog->selectedFiles();
            } else {
                return false;
            }

            return playerTableModel->SaveDraftStatus(files.at(0));
        });
        fileMenu->addAction(saveResultsAction);
        
        // Main Menu > File menu > Load action
        QAction* loadResultsAction = new QAction("&Load Results...", this);
        connect(loadResultsAction, &QAction::triggered, [=](bool checked) {

            auto dialog = GetFileDialog(QFileDialog::AcceptOpen);

            QStringList files;
            if (dialog->exec()) {
                files = dialog->selectedFiles();
            } else {
                return false;
            }

            return playerTableModel->LoadDraftStatus(files.at(0));
        });
        fileMenu->addAction(loadResultsAction);

        // Main Menu > File menu
        QMenu* settingsMenu = mainMenuBar->addMenu("&Settings");

        // Main Menu > Settings menu > Options action
        QAction* optionsAction = new QAction("&Options...", this);
        connect(optionsAction, &QAction::triggered, [=](bool checked) {
            // TODO: Some settings would be nice...
        });
        settingsMenu->addAction(optionsAction);

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
        QLabel {
            font-family: "Consolas";
            font-size: 11px;
        }
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

