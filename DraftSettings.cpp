#include "DraftSettings.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QIntValidator>
#include <QSlider>

DraftSettings& DraftSettings::Get()
{
    static DraftSettings* s_draftSettings = nullptr;

    if (!s_draftSettings) {

        s_draftSettings = new DraftSettings;

        s_draftSettings->Budget = 280;
        s_draftSettings->HitterCount = 14;
        s_draftSettings->PitcherCount = 10;
        s_draftSettings->RosterSize = 24;
        s_draftSettings->HittingSplit = 0.70f;
        s_draftSettings->PitchingSplit = 0.30f;
        s_draftSettings->OwnerCount = 12;
        s_draftSettings->OwnerNames = QStringList({
            "--",
            "The 700 Level",
            "Hoi Pollois",
            "Wooden Spooners",
            "Gopher Balls",
            "Kilroy Sureshots",
            "Warning Track Power",
            "Alien Nation",
            "Phil D Walletup",
            "Has Bens",
            "Young Guns",
            "Master Batters",
            "Steroid Stiffs",
        });
        s_draftSettings->OwnerAbbreviations = QStringList({
            "--",
            "700",
            "HP",
            "WS",
            "GB",
            "KS",
            "WTP",
            "AN",
            "PDW",
            "HB",
            "YG",
            "MB",
            "SS",
        });
    }

    return *s_draftSettings;
}


DraftSettingsDialog::DraftSettingsDialog()
{
    // Cancel button
    QPushButton* cancelButton = new QPushButton("Cancel");
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Draft button
    QPushButton* draftButton = new QPushButton("Save");
    draftButton->setDefault(true);
    connect(draftButton, &QPushButton::clicked, this, &QDialog::accept);

    // Button layout
    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(draftButton);

    // Owner group box
    QGroupBox* ownerGroupBox = new QGroupBox("Owners");
    QFormLayout* ownerForumLayout = new QFormLayout(ownerGroupBox);
    for (auto i = 1u; i <= DraftSettings::Get().OwnerCount; i++) {
        auto* lineEditOwner = new QLineEdit(DraftSettings::Get().OwnerNames[i]);
        ownerForumLayout->addRow(QString("Owner #%1:").arg(i), lineEditOwner);
    }

    // Budget group box
    QGroupBox* budgetGroupBox = new QGroupBox("Budget");
    QFormLayout* budgetForumLayout = new QFormLayout(budgetGroupBox);
    auto* lineEditHitters = new QLineEdit("14");
    lineEditHitters->setValidator(new QIntValidator(0, INT_MAX));
    budgetForumLayout->addRow(tr("# Hitters:"), lineEditHitters);
    auto* lineEditPitchers = new QLineEdit("10");
    lineEditPitchers->setValidator(new QIntValidator(0, INT_MAX));
    budgetForumLayout->addRow(tr("# Pitcher:"), lineEditPitchers);
    auto* lineEditBudget = new QLineEdit("280");
    lineEditBudget->setValidator(new QIntValidator(0, INT_MAX));
    budgetForumLayout->addRow(tr("Budget:"), lineEditBudget);
    auto* hittingSplit = new QLineEdit("70");
    hittingSplit->setValidator(new QIntValidator(0, 100));
    budgetForumLayout->addRow(tr("Hitting Split:"), hittingSplit);

    // Player pool group box
    auto* playerPoolGroupBox = new QGroupBox("Player Pool");
    auto* playerPoolGroupLayout = new QHBoxLayout(playerPoolGroupBox);
    auto* radioNL = new QRadioButton(tr("NL Only"));
    playerPoolGroupLayout->addWidget(radioNL);
    auto* radioAL = new QRadioButton(tr("AL Only"));
    playerPoolGroupLayout->addWidget(radioAL);
    auto* radioMixed = new QRadioButton(tr("Mixed"));
    playerPoolGroupLayout->addWidget(radioMixed);
    radioNL->setChecked(true);

    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(ownerGroupBox);
    mainLayout->addWidget(budgetGroupBox);
    mainLayout->addWidget(playerPoolGroupBox);
    mainLayout->addSpacing(6);
    mainLayout->addLayout(buttonsLayout);

    // Configure this dialog
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Adjust Settings"));
    setLayout(mainLayout);
    setModal(true);
}

