
//------------------------------------------------------------------------------
// LinkDelegate
//------------------------------------------------------------------------------
class LinkDelegate : public QStyledItemDelegate
{
public:

    LinkDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
    {
    }

    QString displayText(const QVariant& value, const QLocale& locale) const override
    {
        return "link";
    }

    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override
    {
        // On mouse release...
        if (event->type() == QEvent::MouseButtonRelease) {

            auto id = model->data(index, Qt::DisplayRole).toString();
            auto link = QString("http://www.fangraphs.com/statss.aspx?playerid=%1").arg(id);
            QDesktopServices::openUrl(link);
        }

        return false;
    }
};

//------------------------------------------------------------------------------
// TagDelegate
//------------------------------------------------------------------------------
class TagDelegate : public QStyledItemDelegate
{
public:

    TagDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
    {
    }

    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override
    {
        // On mouse release...
        if (event->type() == QEvent::MouseButtonRelease) {
            uint32_t i = model->data(index, PlayerTableModel::RawDataRole).toUInt();
            if (++i >= Player::FLAG_COUNT) {
                i = 0;
            }
            model->setData(index, i, PlayerTableModel::RawDataRole);
        }

        if (event->type() == QEvent::MouseMove) {

        }

        return false;
    }
};

//------------------------------------------------------------------------------
// FBBMainWindow
//------------------------------------------------------------------------------
class OLD_MAIN : public QMainWindow
{
public:

    OLD_MAIN(QWidget* pParent = nullptr)
        : QMainWindow(pParent)
    {
        Q_INIT_RESOURCE(Resources);

        // Settings persistence
        ReadSettings();

        // Appearance LUT
        PlayerApperances appearances;

        // Build player table model from file
        PlayerTableModel* playerTableModel = new PlayerTableModel(this);
        playerTableModel->LoadHittingProjections(appearances);
        playerTableModel->LoadPitchingProjections(appearances);
        playerTableModel->CalculateHittingScores();
        playerTableModel->CalculatePitchingScores();
        playerTableModel->InitializeTargetValues();

        // Draft delegate
        DraftDelegate* draftDelegate = new DraftDelegate(playerTableModel);
        LinkDelegate* linkDelegate = new LinkDelegate(this);
        TagDelegate* tagDelegate = new TagDelegate(this);

        // Hitter sort-model
        PlayerSortFilterProxyModel* hitterSortFilterProxyModel = new PlayerSortFilterProxyModel(Player::Hitter);
        hitterSortFilterProxyModel->setSourceModel(playerTableModel);
        hitterSortFilterProxyModel->setSortRole(PlayerTableModel::RawDataRole);

        // Hitter table view
        QTableView* hitterTableView = MakeTableView(hitterSortFilterProxyModel, true, PlayerTableModel::COLUMN_Z);
        hitterTableView->setItemDelegateForColumn(FindColumn(hitterSortFilterProxyModel, PlayerTableModel::COLUMN_DRAFT_BUTTON), draftDelegate);
        hitterTableView->setItemDelegateForColumn(FindColumn(hitterSortFilterProxyModel, PlayerTableModel::COLUMN_ID_LINK), linkDelegate);
        hitterTableView->setItemDelegateForColumn(FindColumn(hitterSortFilterProxyModel, PlayerTableModel::COLUMN_FLAG), tagDelegate);
        hitterTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);

        // Context menu
        QMenu* contextMenu = new QMenu();
        contextMenu->addAction("&Remove Player");

        // Apply to hitter table view
        hitterTableView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(hitterTableView, &QWidget::customContextMenuRequested, [=](const QPoint& pos) {
            QPoint globalPos = hitterTableView->mapToGlobal(pos);
            QAction* selectedItem = contextMenu->exec(globalPos);
            if (selectedItem) {
                auto proxyIndex = hitterTableView->indexAt(pos);
                auto srcIndex = hitterSortFilterProxyModel->mapToSource(proxyIndex);
                playerTableModel->RemovePlayer(srcIndex.row());
            }
        });

        // Pitcher sort-model
        PlayerSortFilterProxyModel* pitcherSortFilterProxyModel = new PlayerSortFilterProxyModel(Player::Pitcher);
        pitcherSortFilterProxyModel->setSourceModel(playerTableModel);
        pitcherSortFilterProxyModel->setSortRole(PlayerTableModel::RawDataRole);
        
        // Pitcher table view
        QTableView* pitcherTableView = MakeTableView(pitcherSortFilterProxyModel, true, PlayerTableModel::COLUMN_Z);
        pitcherTableView->setItemDelegateForColumn(FindColumn(pitcherSortFilterProxyModel, PlayerTableModel::COLUMN_DRAFT_BUTTON), draftDelegate);
        pitcherTableView->setItemDelegateForColumn(FindColumn(pitcherSortFilterProxyModel, PlayerTableModel::COLUMN_ID_LINK), linkDelegate);
        pitcherTableView->setItemDelegateForColumn(FindColumn(pitcherSortFilterProxyModel, PlayerTableModel::COLUMN_FLAG), tagDelegate);
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

        // Drafted filter action
        QAction* filterDrafted = new QAction(this);
        connect(filterDrafted, &QAction::toggled, hitterSortFilterProxyModel, &PlayerSortFilterProxyModel::OnFilterDrafted);
        connect(filterDrafted, &QAction::toggled, pitcherSortFilterProxyModel, &PlayerSortFilterProxyModel::OnFilterDrafted);
        filterDrafted->setText(tr("Drafted"));
        filterDrafted->setToolTip("Toggle Drafted Players");
        filterDrafted->setCheckable(true);
        filterDrafted->toggle();

        QAction* filterReplacement = new QAction(this);
        connect(filterReplacement, &QAction::toggled, hitterSortFilterProxyModel, &PlayerSortFilterProxyModel::OnFilterReplacement);
        connect(filterReplacement, &QAction::toggled, pitcherSortFilterProxyModel, &PlayerSortFilterProxyModel::OnFilterReplacement);
        filterReplacement->setText(tr("($1)"));
        filterReplacement->setToolTip("Toggle replacements players with value under $1");
        filterReplacement->setCheckable(true);
        filterReplacement->toggle();

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
        toolbar->addWidget(new QLabel(" Status: ", this));
        toolbar->addActions(QList<QAction*>{filterDrafted, filterReplacement});
        toolbar->addSeparator();
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

        // Owner labels
        QList<QLabel*>* pVecOwnerLabels;
        pVecOwnerLabels = new QList<QLabel*>();
        pVecOwnerLabels->append(new QLabel("--"));
        for (auto i = 1u; i <= DraftSettings::Get().OwnerCount; i++) {
            pVecOwnerLabels->append(new QLabel(DraftSettings::Get().OwnerNames[i]));
        }

        // Update label helper
        auto UpdateOwnerLabels = [=]() {
            for (auto i = 1u; i <= DraftSettings::Get().OwnerCount; i++) {
                pVecOwnerLabels->at(i)->setText(DraftSettings::Get().OwnerNames[i]);
            }
        };

        // Initialize
        UpdateOwnerLabels();

        // Loop owners
        for (uint32_t ownerId = 1; ownerId <= DraftSettings::Get().OwnerCount; ownerId++) {

            // V-Layout per owner
            QVBoxLayout* perOwnerLayout = new QVBoxLayout(this);
            ownersLayout->addLayout(perOwnerLayout);
            perOwnerLayout->setSizeConstraint(QLayout::SetNoConstraint);

            // Proxy model for this owner
            OwnerSortFilterProxyModel* ownerSortFilterProxyModel = new OwnerSortFilterProxyModel(ownerId, playerTableModel, this);
            vecOwnerSortFilterProxyModels.push_back(ownerSortFilterProxyModel);

            // Owner name label
            pVecOwnerLabels->at(ownerId)->setAlignment(Qt::AlignCenter);
            perOwnerLayout->addWidget(pVecOwnerLabels->at(ownerId));

            // Per-owner roster table view
            const uint32_t tableWidth = 225;
            QTableView* ownerRosterTableView = MakeTableView(ownerSortFilterProxyModel, true, 0);
            ownerRosterTableView->setMinimumSize(tableWidth, 65);
            ownerRosterTableView->setMaximumSize(tableWidth, 4096);
            ownerRosterTableView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            perOwnerLayout->addWidget(ownerRosterTableView);

            // XXX: This should be a form layout...
            QGridLayout* ownerSummaryGridLayout = new QGridLayout(this);
            ownerSummaryGridLayout->setSpacing(0);
            ownerSummaryGridLayout->addWidget(MakeLabel("Budget: "),     0, 0);
            ownerSummaryGridLayout->addWidget(MakeLabel("# Hitters: "),  1, 0);
            ownerSummaryGridLayout->addWidget(MakeLabel("# Pitchers: "), 2, 0);
            ownerSummaryGridLayout->addWidget(MakeLabel("Max Bid: "),    3, 0);

            QLabel* budgetLabel = MakeLabel();
            QLabel* numHittersLabel = MakeLabel();
            QLabel* numPitchersLabel = MakeLabel();
            QLabel* maxBidLabel = MakeLabel();

            // Helper
            auto UpdateLabels = [=]()
            {
                budgetLabel->setText(QString("$%1").arg(ownerSortFilterProxyModel->GetRemainingBudget()));
                numHittersLabel->setText(QString("%1 / %2").arg(ownerSortFilterProxyModel->Count(Player::Hitter)).arg(DraftSettings::Get().HitterCount));
                numPitchersLabel->setText(QString("%1 / %2").arg(ownerSortFilterProxyModel->Count(Player::Pitcher)).arg(DraftSettings::Get().PitcherCount));
                maxBidLabel->setText(QString("$%1").arg(ownerSortFilterProxyModel->GetMaxBid()));
            };

            // Update labels when a draft event happens
            connect(playerTableModel, &PlayerTableModel::DraftedEnd, [=]() {
                UpdateLabels();
            });

            UpdateLabels();

            ownerSummaryGridLayout->addWidget(budgetLabel,      0, 1);
            ownerSummaryGridLayout->addWidget(numHittersLabel,  1, 1);
            ownerSummaryGridLayout->addWidget(numPitchersLabel, 2, 1);
            ownerSummaryGridLayout->addWidget(maxBidLabel,      3, 1);

            QSpacerItem* spacer = new QSpacerItem(1, 1, QSizePolicy::Preferred, QSizePolicy::Preferred);

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

        // Target value widget
        QWidget* targetValueWidget = new QWidget(this);
        QFormLayout* targetValueLayout = new QFormLayout(this);
        targetValueWidget->setLayout(targetValueLayout);
        auto values = {
            PlayerTableModel::COLUMN_AVG,
            PlayerTableModel::COLUMN_HR,
            PlayerTableModel::COLUMN_R,
            PlayerTableModel::COLUMN_RBI,
            PlayerTableModel::COLUMN_SB,
            PlayerTableModel::COLUMN_SO,
            PlayerTableModel::COLUMN_ERA,
            PlayerTableModel::COLUMN_WHIP,
            PlayerTableModel::COLUMN_W,
            PlayerTableModel::COLUMN_SV,
        };
        for (auto value : values) {
            auto name = playerTableModel->headerData(value, Qt::Horizontal, Qt::DisplayRole).toString();
            auto target = QString::number(playerTableModel->GetTargetValue(value), 'f', 3);
            targetValueLayout->addRow(name, new QLabel(target));
        }

        // Player scatter plot
        PlayerScatterPlotChart* chartView = new PlayerScatterPlotChart(playerTableModel, hitterSortFilterProxyModel, this);
        connect(hitterSortFilterProxyModel,  &QSortFilterProxyModel::layoutChanged, chartView, &PlayerScatterPlotChart::Update);
        connect(pitcherSortFilterProxyModel, &QSortFilterProxyModel::layoutChanged, chartView, &PlayerScatterPlotChart::Update);
        connect(playerTableModel, &QAbstractItemModel::dataChanged, chartView, &PlayerScatterPlotChart::Update);

        // Summary view
        SummaryWidget* summary = new SummaryWidget(playerTableModel, vecOwnerSortFilterProxyModels, this);

        // Bottom tabs
        enum BottomSectionTabs { Rosters, Summary, Targets, ChartView, Log };
        QTabWidget* bottomTabs = new QTabWidget(this);
        topBottomSplitter->addWidget(bottomTabs);
        bottomTabs->insertTab(BottomSectionTabs::Rosters, ownerScrollArea, "Rosters");
        bottomTabs->insertTab(BottomSectionTabs::Summary, summary, "Summary");
        bottomTabs->insertTab(BottomSectionTabs::Targets, targetValueWidget, "Targets");
        bottomTabs->insertTab(BottomSectionTabs::ChartView, chartView, "Scatter Chart");
        bottomTabs->insertTab(BottomSectionTabs::Log, GlobalLogger::Get(), "Log");

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

        // Ask for the save location 
        auto SetSaveAsFile = [=]()
        {
            QStringList files;
            auto dialog = GetFileDialog(QFileDialog::AcceptSave);
            if (dialog->exec()) {
                files = dialog->selectedFiles();
            } else {
                return false;
            }
            m_currentFile = files.at(0);
            return true;
        };

        // Update title bar
        auto UpdateApplicationName = [this]()
        {
            auto name = QString("fbb -- %1").arg(QFileInfo(m_currentFile).fileName());
            QCoreApplication::setApplicationName(name);
            setWindowTitle(name);
        };

        // Main Menu > File menu > Save action
        QAction* saveResultsAction = new QAction("&Save Results", this);
        connect(saveResultsAction, &QAction::triggered, [=](bool checked) {
            if (m_currentFile.isEmpty()) {
                SetSaveAsFile();
            }
            GlobalLogger::AppendMessage(QString("Saving file: %1...").arg(m_currentFile));
            UpdateApplicationName();
            return playerTableModel->SaveDraftStatus(m_currentFile);
        });
        fileMenu->addAction(saveResultsAction);

        // Main Menu > File menu > Save As action
        QAction* saveResultsAsAction = new QAction("Save Results &As...", this);
        connect(saveResultsAsAction, &QAction::triggered, [=](bool checked) {
            SetSaveAsFile();
            GlobalLogger::AppendMessage(QString("Saving file: %1...").arg(m_currentFile));
            UpdateApplicationName();
            return playerTableModel->SaveDraftStatus(m_currentFile);
        });
        fileMenu->addAction(saveResultsAsAction);
        
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
            m_currentFile = files.at(0);
            GlobalLogger::AppendMessage(QString("Loading file: %1...").arg(m_currentFile));
            UpdateApplicationName();
            return playerTableModel->LoadDraftStatus(m_currentFile);
        });
        fileMenu->addAction(loadResultsAction);

        // Main Menu > File menu
        QMenu* settingsMenu = mainMenuBar->addMenu("&Settings");

        // Main Menu > Settings menu > Options action
        QAction* settingsAction = new QAction("&Settings...", this);
        connect(settingsAction, &QAction::triggered, [=](bool checked) {
            DraftSettingsDialog draftSettingsDialog;
            if (draftSettingsDialog.exec()) {
                UpdateOwnerLabels();
            }
        });
        settingsMenu->addAction(settingsAction);

        // Main Menu > Settings menu > Options action
        QAction* demoDataAction = new QAction("&DemoData...", this);
        connect(demoDataAction, &QAction::triggered, [=](bool checked) {
            playerTableModel->DraftRandom();
        });
        settingsMenu->addAction(demoDataAction);

        // show me
        QMainWindow::show();
    }

private:

    // Owner names
    QVector<QString> m_vecOwners;

    // Last "Save As..." file
    QString m_currentFile;

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
        event->ignore();

        QMessageBox closeWarning;
        closeWarning.setText(QString("Are you sure you want to quit?"));
        closeWarning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        closeWarning.setDefaultButton(QMessageBox::No);
        if (closeWarning.exec() == QMessageBox::Yes) {
            WriteSettings();
            event->accept();
        }
    }

    // Table factory
    QTableView* MakeTableView(QAbstractItemModel* model, bool sortingEnabled = true, uint32_t modelSortColumn = 0)
    {
        class MyTable : public QTableView
        {
        public:
            QStyleOptionViewItem viewOptions() const override
            {
                QStyleOptionViewItem option = QTableView::viewOptions();
                option.decorationAlignment = Qt::AlignHCenter | Qt::AlignCenter;
                option.decorationPosition = QStyleOptionViewItem::Top;
                return option;
            }

            void mouseMoveEvent(QMouseEvent* event) override
            {
                QModelIndex index = indexAt(event->pos());
                if (index.isValid()) {
                    QVariant data = model()->data(index, PlayerTableModel::CursorRole);
                    Qt::CursorShape shape = Qt::ArrowCursor;
                    if (!data.isNull()) {
                        shape = Qt::CursorShape(data.toInt());
                    }
                    setCursor(shape);
                }

                QTableView::mouseMoveEvent(event);
            }
        };

        QTableView* tableView = new MyTable();
        tableView->setModel(model);
        tableView->setSortingEnabled(sortingEnabled);
        if (sortingEnabled) {
            tableView->sortByColumn(FindColumn(model, modelSortColumn));
        }
        tableView->verticalHeader()->hide();
        tableView->setAlternatingRowColors(true);
        tableView->verticalHeader()->setDefaultSectionSize(15);
        tableView->resizeColumnsToContents();
        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView->setFocusPolicy(Qt::StrongFocus);

        // enable mouse tracking
        tableView->setMouseTracking(true);
        tableView->viewport()->setMouseTracking(true);
        tableView->installEventFilter(this);
        tableView->viewport()->installEventFilter(this);

        return tableView;
    }


    static QLabel* MakeLabel(const QString& text = QString())
    {
        QLabel* label = new QLabel(text);

        auto r = 
            R"""(
            QLabel{
                font-family: "Consolas";
                font-size: 11px;
            }
            )""";

        label->setStyleSheet(r);
        return label;
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
