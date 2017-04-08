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

FBBLeaugeSettingsDialog::FBBLeaugeSettingsDialog(QWidget* pParent)
    : QDialog(pParent)
{
    // Default settings
    // TODO: this needs to be loaded from somewhere...
    FBBLeaugeSettings settings;

    // Title
    setWindowTitle("Leauge Settings");
    
    // Sizing
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Main layout
    QVBoxLayout* pLayout = new QVBoxLayout(this);

    // Tabs
    QTabWidget* pTabWidget = new QTabWidget();
    pLayout->addWidget(pTabWidget);

    // Leauge
    {
        QWidget* pTab = new QWidget();
        QVBoxLayout* pTabLayout = new QVBoxLayout(pTab);
        QLabel* pDescription = new QLabel("Indicate whether your league only allows players from specific teams", pTab);
        QFormLayout* pForm = new QFormLayout();
        QComboBox* pTypeCombo = new QComboBox();

        pForm->addRow("Type:", pTypeCombo);

        pTypeCombo->addItem("Mixed");
        pTypeCombo->addItem("NL Only");
        pTypeCombo->addItem("AL Only");
        pTypeCombo->setCurrentIndex(uint32_t(settings.leauge.type));

        connect(pTypeCombo, static_cast<void (QComboBox::*)(int32_t)>(&QComboBox::currentIndexChanged), this, [&](int32_t index) {
            settings.leauge.type = FBBLeaugeSettings::Leauge::Type(index);
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

        AddValue(pHittingForm, "AVG:", settings.categories.hitting.AVG);
        AddValue(pHittingForm, "RBI:", settings.categories.hitting.RBI);
        AddValue(pHittingForm, "R:", settings.categories.hitting.R);
        AddValue(pHittingForm, "SB:", settings.categories.hitting.SB);
        AddValue(pHittingForm, "HR:", settings.categories.hitting.HR);

        AddValue(pPitchingForm, "W:", settings.categories.pitching.W);
        AddValue(pPitchingForm, "SV:", settings.categories.pitching.SV);
        AddValue(pPitchingForm, "ERA:", settings.categories.pitching.ERA);
        AddValue(pPitchingForm, "WHIP:", settings.categories.pitching.WHIP);
        AddValue(pPitchingForm, "SO:", settings.categories.pitching.SO);

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
        
        pGroupLayout->setContentsMargins(0, 0, 0, 0);
        pGroupLayout->addWidget(pHittingGrounp);
        pGroupLayout->addWidget(pPitchingGroup);
        pGroupLayout->addStretch();

        // Helper to add a new value
        auto AddValue = [this](QFormLayout* pForm, const char* name, uint32_t& setting) {
            QLineEdit* pLineEdit = new QLineEdit();
            pLineEdit->setFixedWidth(25);
            pLineEdit->setValidator(new QIntValidator(0, 99, this));
            pLineEdit->setText(QString::number(setting));
            pForm->addRow(QString(name), pLineEdit);
            connect(pLineEdit, &QLineEdit::textEdited, [&](const QString& text) {
                setting = text.toUInt();
            });
        };

        // Hitting values
        AddValue(pHittingForm, "C:", settings.positions.hitting.numC);
        AddValue(pHittingForm, "1B:", settings.positions.hitting.num1B);
        AddValue(pHittingForm, "2B:", settings.positions.hitting.num2B);
        AddValue(pHittingForm, "SS:", settings.positions.hitting.numSS);
        AddValue(pHittingForm, "3B:", settings.positions.hitting.num3B);
        AddValue(pHittingForm, "OF:", settings.positions.hitting.numOF);
        AddValue(pHittingForm, "MI:", settings.positions.hitting.numMI);
        AddValue(pHittingForm, "CI:", settings.positions.hitting.numCI);
        AddValue(pHittingForm, "IF:", settings.positions.hitting.numIF);
        AddValue(pHittingForm, "UT:", settings.positions.hitting.numUT);
        
        // Pitching values
        AddValue(pPitchingForm, "SP:", settings.positions.pitching.numSP);
        AddValue(pPitchingForm, "RP:", settings.positions.pitching.numRP);
        AddValue(pPitchingForm, "P:", settings.positions.pitching.numP);

        pTabLayout->addWidget(pDescription);
        pTabLayout->addWidget(pGroups);
        pTabLayout->addStretch();

        pTabWidget->addTab(pTab, "Positions");
    }

    // Team settings
    {
        QWidget* pOwners = new QWidget();
        pTabWidget->addTab(pOwners, "Owners");
    }

    // Team settings
    {
        QWidget* pBuget = new QWidget();
        pTabWidget->addTab(pBuget, "Budget");
    }

    // Projections
    {
        QWidget* pProjections = new QWidget();
        pTabWidget->addTab(pProjections, "Projections");
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

        pLayout->addWidget(pButtons);
    }
}
