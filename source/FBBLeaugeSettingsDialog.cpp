#include "FBB/FBBLeaugeSettingsDialog.h"
#include "FBB/FBBLeaugeSettings.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QTableView>
#include <QAbstractTableModel>
#include <QAbstractItemDelegate>
#include <QVariant>
#include <QApplication>
#include <QMessageBox>
#include <QSlider>
#include <QSpinBox>

static void BuildOwnerLayout(QGridLayout* pOwnerLayout, FBBLeaugeSettings* pSettings)
{
    // Clear layout
    // TODO: Revisit this... I don't love this
    QLayoutItem* item;
    while ((item = pOwnerLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Header
    int row = 0;
    pOwnerLayout->addWidget(new QLabel("Team Name"), row, 1);
    pOwnerLayout->addWidget(new QLabel("Abbreviation"), row, 2);

    QList<FBBOwnerId> keys = pSettings->owners.keys();

    // Repopulate
    // TODO: Can we loop by pairs like the STL?
    for (FBBOwnerId& key : keys) {

        std::shared_ptr<FBBLeaugeSettings::Owner>& spOwner = pSettings->owners[key];
        row++;

        QLabel* pRow = new QLabel("#" + QString::number(row));
        QLineEdit* pTeamName = new QLineEdit(spOwner->name);
        QLineEdit* pTeamAbbreviation = new QLineEdit(spOwner->abbreviation);
        QPushButton* pDeleteButton = new QPushButton(QIcon(":/icons/trash_16x16.png"), QString());

        pRow->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        pTeamName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        pTeamAbbreviation->setMaxLength(3);
        pTeamAbbreviation->setMaximumWidth(75);
        pTeamAbbreviation->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        // pDeleteButton->setMaximumWidth(25);
        pDeleteButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        if (pSettings->owners.count() == 1) {
            pDeleteButton->setEnabled(false);
        }

        if (spOwner->name.isEmpty()) {
            pTeamName->setPlaceholderText("Enter team name...");
        }

        if (spOwner->abbreviation.isEmpty()) {
            pTeamAbbreviation->setPlaceholderText("ABC");
        }

        QObject::connect(pTeamName, &QLineEdit::editingFinished, [=]() {
            pSettings->owners[key]->name = pTeamName->text();
        });

        QObject::connect(pTeamAbbreviation, &QLineEdit::editingFinished, [=]() {
            pSettings->owners[key]->abbreviation = pTeamAbbreviation->text();
        });

        QObject::connect(pDeleteButton, &QPushButton::pressed, [=]() {

            if (QMessageBox::question(nullptr, qApp->applicationName(), "Are you sure you want to delete this owner?") == QMessageBox::Yes) {
                pSettings->owners.remove(key);
                BuildOwnerLayout(pOwnerLayout, pSettings);
            }
        });

        pOwnerLayout->addWidget(pRow, row, 0);
        pOwnerLayout->addWidget(pTeamName, row, 1);
        pOwnerLayout->addWidget(pTeamAbbreviation, row, 2);
        pOwnerLayout->addWidget(pDeleteButton, row, 3);
    }
};


FBBLeaugeSettingsDialog::FBBLeaugeSettingsDialog(FBBLeaugeSettings* pSettings, QWidget* pParent)
    : QDialog(pParent)
{
    setWindowTitle("Leauge Settings");

    // Big enough to show all of the tabs
    setMinimumWidth(500);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QVBoxLayout* pLayout = new QVBoxLayout(this);

    QTabWidget* pTabWidget = new QTabWidget();
    pLayout->addWidget(pTabWidget);

    // Leauge
    {
        QWidget* pTab = new QWidget();
        QVBoxLayout* pTabLayout = new QVBoxLayout(pTab);
        QLabel* pDescription = new QLabel("Indicate whether your league only allows players from specific teams", pTab);
        QFormLayout* pForm = new QFormLayout();
        QComboBox* pTypeCombo = new QComboBox();
        QLineEdit* pBudget = new QLineEdit();
        QLineEdit* pPositionEligibility = new QLineEdit();

        pTypeCombo->addItem("Mixed");
        pTypeCombo->addItem("NL Only");
        pTypeCombo->addItem("AL Only");
        pTypeCombo->setCurrentIndex(uint32_t(pSettings->leauge.type));

        pBudget->setValidator(new QIntValidator(0, UINT32_MAX, this));
        pBudget->setText(QString::number(pSettings->leauge.budget));

        pPositionEligibility->setValidator(new QIntValidator(0, 100, this));
        pPositionEligibility->setText(QString::number(pSettings->leauge.positionEligibility));

        pForm->addRow("Type:", pTypeCombo);
        pForm->addRow("Budget ($):", pBudget);
        pForm->addRow("Position Eligibility:", pPositionEligibility);

        connect(pTypeCombo, static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged), this, [=](int32_t index) {
            pSettings->leauge.type = FBBLeaugeSettings::Leauge::Type(index);
        });

        connect(pBudget, &QLineEdit::textEdited, [=](const QString& text) {
            pSettings->leauge.budget = text.toUInt();
        });

        connect(pPositionEligibility, &QLineEdit::textEdited, [=](const QString& text) {
            pSettings->leauge.positionEligibility = text.toUInt();
        });

        pTabLayout->addWidget(pDescription);
        pTabLayout->addLayout(pForm);
        pTabLayout->addStretch();

        pTabWidget->addTab(pTab, "Leauge");
    }

    // Categories
    {
        QWidget* pTab = new QWidget();
        QVBoxLayout* pTabLayout = new QVBoxLayout(pTab);
        QWidget* pGroups = new QWidget();
        QHBoxLayout* pGroupLayout = new QHBoxLayout(pGroups);
        QLabel* pDescription = new QLabel("Select the roto categories used to score your leauge");
        QGroupBox* pHittingGrounp = new QGroupBox("Hitting");
        QFormLayout* pHittingForm = new QFormLayout(pHittingGrounp);
        QGroupBox* pPitchingGroup = new QGroupBox("Pitching");
        QFormLayout* pPitchingForm = new QFormLayout(pPitchingGroup);

        // Helper to add a new value
        auto AddValue = [this](QFormLayout* pForm, const char* name, bool& setting) {
            QCheckBox* pCheckBox = new QCheckBox();
            pCheckBox->setCheckState(setting ? Qt::Checked : Qt::Unchecked);
            pForm->addRow(QString(name), pCheckBox);
            connect(pCheckBox, &QCheckBox::stateChanged, [&](int state) {
                setting = (state == Qt::Checked ? true : false);
            });
        };

        pGroupLayout->setContentsMargins(0, 0, 0, 0);
        pGroupLayout->addWidget(pHittingGrounp);
        pGroupLayout->addWidget(pPitchingGroup);
        pGroupLayout->addStretch();

        AddValue(pHittingForm, "AVG:", pSettings->categories.hitting.AVG);
        AddValue(pHittingForm, "RBI:", pSettings->categories.hitting.RBI);
        AddValue(pHittingForm, "R:", pSettings->categories.hitting.R);
        AddValue(pHittingForm, "SB:", pSettings->categories.hitting.SB);
        AddValue(pHittingForm, "HR:", pSettings->categories.hitting.HR);

        AddValue(pPitchingForm, "W:", pSettings->categories.pitching.W);
        AddValue(pPitchingForm, "SV:", pSettings->categories.pitching.SV);
        AddValue(pPitchingForm, "ERA:", pSettings->categories.pitching.ERA);
        AddValue(pPitchingForm, "WHIP:", pSettings->categories.pitching.WHIP);
        AddValue(pPitchingForm, "SO:", pSettings->categories.pitching.SO);

        pTabLayout->addWidget(pDescription);
        pTabLayout->addWidget(pGroups);
        pTabLayout->addStretch();

        pTabWidget->addTab(pTab, "Categories");
    }

    // Positions
    {
        QWidget* pTab = new QWidget();
        QVBoxLayout* pTabLayout = new QVBoxLayout(pTab);
        QWidget* pGroups = new QWidget();
        QHBoxLayout* pGroupLayout = new QHBoxLayout(pGroups);
        QLabel* pDescription = new QLabel("Select the positions used in your leauge");
        QGroupBox* pHittingGrounp = new QGroupBox("Hitting");
        QFormLayout* pHittingForm = new QFormLayout(pHittingGrounp);
        QGroupBox* pPitchingGroup = new QGroupBox("Pitching");
        QFormLayout* pPitchingForm = new QFormLayout(pPitchingGroup);
        QGroupBox* pOtherGroup = new QGroupBox("Other");
        QFormLayout* pOtherForm = new QFormLayout(pOtherGroup);
        QGroupBox* pSumGroup = new QGroupBox("Totals");
        QFormLayout* pSumForm = new QFormLayout(pSumGroup);

        QLabel* pSumHitters = new QLabel( QString::number(pSettings->SumHitters()));
        QLabel* pSumPitchers = new QLabel(QString::number(pSettings->SumPitchers()));
        QLabel* pSumPlayers = new QLabel( QString::number(pSettings->SumPlayers()));

        pGroupLayout->setContentsMargins(0, 0, 0, 0);
        pGroupLayout->addWidget(pHittingGrounp);
        pGroupLayout->addWidget(pPitchingGroup);
        pGroupLayout->addWidget(pOtherGroup);
        pGroupLayout->addStretch();

        // Helper to add a new value
        auto AddValue = [=](QFormLayout* pForm, const char* name, uint32_t& setting) {
            QSpinBox* pSpinBox = new QSpinBox();
            pSpinBox->setMinimum(0);
            pSpinBox->setValue(setting);
            pSpinBox->setMaximum(99);
            pForm->addRow(QString(name), pSpinBox);
            connect(pSpinBox, static_cast<void (QSpinBox::*)(int32_t)>(&QSpinBox::valueChanged), [=, &setting](int value) {
                setting = value;
                pSumHitters->setText(QString::number(pSettings->SumHitters()));
                pSumPitchers->setText(QString::number(pSettings->SumPitchers()));
                pSumPlayers->setText(QString::number(pSettings->SumPlayers()));
            });
        };

        // Hitting values
        AddValue(pHittingForm, "C:", pSettings->positions.hitting.numC);
        AddValue(pHittingForm, "1B:", pSettings->positions.hitting.num1B);
        AddValue(pHittingForm, "2B:", pSettings->positions.hitting.num2B);
        AddValue(pHittingForm, "SS:", pSettings->positions.hitting.numSS);
        AddValue(pHittingForm, "3B:", pSettings->positions.hitting.num3B);
        AddValue(pHittingForm, "OF:", pSettings->positions.hitting.numOF);
        AddValue(pHittingForm, "MI:", pSettings->positions.hitting.numMI);
        AddValue(pHittingForm, "CI:", pSettings->positions.hitting.numCI);
        AddValue(pHittingForm, "IF:", pSettings->positions.hitting.numIF);
        AddValue(pHittingForm, "LF:", pSettings->positions.hitting.numLF);
        AddValue(pHittingForm, "CF:", pSettings->positions.hitting.numCF);
        AddValue(pHittingForm, "RF:", pSettings->positions.hitting.numRF);
        AddValue(pHittingForm, "DH:", pSettings->positions.hitting.numDH);
        AddValue(pHittingForm, "UT:", pSettings->positions.hitting.numU);

        // Pitching values
        AddValue(pPitchingForm, "SP:", pSettings->positions.pitching.numSP);
        AddValue(pPitchingForm, "RP:", pSettings->positions.pitching.numRP);
        AddValue(pPitchingForm, "P:", pSettings->positions.pitching.numP);

        // Bench
        AddValue(pOtherForm, "Bench:", pSettings->positions.numBench);

        // Sums
        pSumForm->addRow("# Hitters:", pSumHitters);
        pSumForm->addRow("# Pitchers:", pSumPitchers);
        pSumForm->addRow("# Players:", pSumPlayers);

        pTabLayout->addWidget(pDescription);
        pTabLayout->addWidget(pGroups);
        pTabLayout->addWidget(pSumGroup);
        pTabLayout->addStretch();

        pTabWidget->addTab(pTab, "Positions");
    }

    // Owner settings
    {
        QWidget* pTab = new QWidget();
        QVBoxLayout* pTabLayout = new QVBoxLayout(pTab);
        QLabel* pDescription = new QLabel("Information about the teams/owners in the leauge", pTab);
        QPushButton* pAddOwnerButton = new QPushButton("Add Owner");
        QWidget* pOwners = new QWidget();
        QGridLayout* pOwnerLayout = new QGridLayout(pOwners);
        QHBoxLayout* pButtonLayout = new QHBoxLayout();

        pOwnerLayout->setContentsMargins(0, 0, 0, 0);

        pButtonLayout->addStretch();
        pButtonLayout->addWidget(pAddOwnerButton);

        pTabLayout->addWidget(pDescription);
        pTabLayout->addWidget(pOwners);
        pTabLayout->addLayout(pButtonLayout);
        pTabLayout->addStretch();

        connect(pAddOwnerButton, &QPushButton::pressed, [=]() {
            pSettings->CreateOwner();
            BuildOwnerLayout(pOwnerLayout, pSettings);
        });

        // Add some defaults if this is empty 
        if (pSettings->owners.count() == 0) {
            for (uint32_t i = 0; i < 8; i++) {
                pSettings->CreateOwner();
            }
        }

        BuildOwnerLayout(pOwnerLayout, pSettings);

        pTabWidget->addTab(pTab, "Owners");
    }

    // Projections settings
    {
        QWidget* pTab = new QWidget();
        QVBoxLayout* pTabLayout = new QVBoxLayout(pTab);
        QLabel* pDescription = new QLabel("Configuration projections and cost estimates", pTab);
        QFormLayout* pFormLayout = new QFormLayout();
        QComboBox* pSourceComboBox = new QComboBox();
        QWidget* pSplit = new QWidget();
        QHBoxLayout* pSplitLayout = new QHBoxLayout(pSplit);
        QSlider* pSplitSlider = new QSlider();
        QLabel* pSplitValue = new QLabel();
        QLineEdit* pMinAB = new QLineEdit();
        QLineEdit* pMinIP = new QLineEdit();
        QCheckBox* pIncludeFA = new QCheckBox();

        pSourceComboBox->addItem("ZiPS");
        pSourceComboBox->addItem("Fans");
        pSourceComboBox->addItem("Steamer");
        pSourceComboBox->addItem("Depth Charts");
        pSourceComboBox->addItem("ATC");
        pSourceComboBox->setCurrentIndex(uint32_t(pSettings->projections.source));

        pSplitSlider->setOrientation(Qt::Horizontal);
        pSplitSlider->setMinimum(0);
        pSplitSlider->setMaximum(100);
        pSplitSlider->setValue(pSettings->projections.hittingPitchingSplit * 100);

        pSplitLayout->addWidget(pSplitSlider);
        pSplitLayout->addWidget(pSplitValue);
        pSplitLayout->setContentsMargins(0, 0, 0, 0);

        pSplitValue->setText(QString::number(pSettings->projections.hittingPitchingSplit * 100) + "%");

        pMinAB->setValidator(new QIntValidator(0, 999, this));
        pMinAB->setText(QString::number(pSettings->projections.minAB));

        pMinIP->setValidator(new QIntValidator(0, 999, this));
        pMinIP->setText(QString::number(pSettings->projections.minIP));

        pIncludeFA->setCheckState(pSettings->projections.includeFA ? Qt::Checked : Qt::Unchecked);

        connect(pSourceComboBox, static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged), this, [=](int32_t index) {
            pSettings->projections.source = FBBLeaugeSettings::Projections::Source(index);
        });

        connect(pSplitSlider, &QSlider::valueChanged, this, [=](int32_t value) {
            pSettings->projections.hittingPitchingSplit = value / 100.f;
            pSplitValue->setText(QString::number(value) + "%");
        });

        connect(pMinAB, &QLineEdit::editingFinished, [=]() {
            pSettings->projections.minAB = pMinAB->text().toUInt();
        });

        connect(pMinIP, &QLineEdit::editingFinished, [=]() {
            pSettings->projections.minIP = pMinIP->text().toUInt();
        });

        connect(pIncludeFA, &QCheckBox::stateChanged, [=](const int& state) {
            pSettings->projections.includeFA = (state == Qt::Checked ? true : false);
        });

        pFormLayout->addRow("Source:", pSourceComboBox);
        pFormLayout->addRow("Hitting-to-Pitching Split:", pSplit);
        pFormLayout->addRow("Minimum AB:", pMinAB);
        pFormLayout->addRow("Minimum IP:", pMinIP);
        pFormLayout->addRow("Include FAs:", pIncludeFA);

        pTabLayout->addWidget(pDescription);
        pTabLayout->addLayout(pFormLayout);
        pTabLayout->addStretch();

        pTabWidget->addTab(pTab, "Projections");
    }

    // Buttons
    {
        QWidget* pButtons = new QWidget();
        QHBoxLayout* pButtonsLayout = new QHBoxLayout(pButtons);
        QPushButton* pCancelButton = new QPushButton("Cancel");
        QPushButton* pAcceptButton = new QPushButton("Save");

        pButtonsLayout->setContentsMargins(0, 0, 0, 0);

        pAcceptButton->setDefault(true);

        pButtonsLayout->addStretch();
        pButtonsLayout->addWidget(pCancelButton);
        pButtonsLayout->addWidget(pAcceptButton);

        connect(pAcceptButton, &QPushButton::pressed, this, &QDialog::accept);
        connect(pCancelButton, &QPushButton::pressed, this, &QDialog::reject);

        pLayout->addWidget(pButtons);
    }
}
