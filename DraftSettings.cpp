#include "DraftSettings.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QIntValidator>
#include <QLabel>
#include <QCheckBox>
#include <array>

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
        s_draftSettings->PitchingSplit = 1.f - s_draftSettings->HittingSplit;
        // s_draftSettings->OwnerCount = 12;
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
            "",
            "",
            "",
            "",
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
            "",
            "",
            "",
            "",
        });
    }

    return *s_draftSettings;
}


DraftSettingsDialog::DraftSettingsDialog()
{
    // Cancel button
    QPushButton* cancelButton = new QPushButton("Cancel");

    // Draft button
    QPushButton* acceptButton = new QPushButton("Save");
    acceptButton->setDefault(true);

    // Button layout
    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(acceptButton);

    // 
    QList<QLineEdit*> vecNames;
    QList<QLineEdit*> vecAbbreviations;

    //
    enum OwnerGridColumn
    {
        OwnerGridColumn_ID,
        OwnerGridColumn_Name,
        OwnerGridColumn_Abbreviateion,
        OwnerGridColumn_COUNT
    };

    // Column names
    const char* ColumnNames[] = {
        "ID",
        "Owner Name",
        "Abbreviation"
    };

    // Owner group box
    QGroupBox* ownerGroupBox = new QGroupBox("Owners");
    QGridLayout* ownerGridLayout = new QGridLayout(ownerGroupBox);

    // Per max owner...
    for (auto row = 0u; row <= DraftSettings::Get().OwnerCount; row++) {

        // Per column...
        for (auto col = 0u; col < OwnerGridColumn_COUNT; col++) {

            // Header
            if (row == 0) {
                ownerGridLayout->addWidget(new QLabel(ColumnNames[col]), row, col);
                continue;
            }

            // Widget data
            auto GetWidget = [&]() -> QWidget*
            {
                const bool valid = row <= DraftSettings::Get().OwnerCount;

                switch (col)
                {
                case OwnerGridColumn_ID:
                    return new QLabel(QString("#%1").arg(row));
                case OwnerGridColumn_Name:
                    {
                        auto lineEdit = valid ? new QLineEdit(DraftSettings::Get().OwnerNames[row]) : new QLineEdit();
                        vecNames.append(lineEdit);
                        return lineEdit;
                    }
                case OwnerGridColumn_Abbreviateion:
                    {
                        auto lineEdit = valid ? new QLineEdit(DraftSettings::Get().OwnerAbbreviations[row]) : new QLineEdit();
                        vecAbbreviations.append(lineEdit);
                        return lineEdit;
                    }
                default:
                    return nullptr;
                    break;
                }
            };

            ownerGridLayout->addWidget(GetWidget(), row, col);
        }
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

    // Just returns
    connect(cancelButton, &QPushButton::clicked, [=]{
        reject();
    });

    // Save settings
    connect(acceptButton, &QPushButton::clicked, [=] {

        // Basic settings
        DraftSettings draftSettings = {};
        draftSettings.Budget = lineEditBudget->text().toInt();
        draftSettings.HitterCount = lineEditHitters->text().toInt();
        draftSettings.PitcherCount = lineEditPitchers->text().toInt();
        draftSettings.RosterSize = draftSettings.HitterCount + draftSettings.PitcherCount;
        draftSettings.HittingSplit = hittingSplit->text().toFloat() / 100.f;
        draftSettings.PitchingSplit = 1.f - draftSettings.HittingSplit;

        // Owner 0
        draftSettings.OwnerNames.append(DraftSettings::Get().OwnerNames[0]);
        draftSettings.OwnerAbbreviations.append(DraftSettings::Get().OwnerAbbreviations[0]);

        // Loop owners
        for (auto i = 0u; i < DraftSettings::Get().OwnerCount; i++) {
            draftSettings.OwnerNames.append(vecNames[i]->text());
            draftSettings.OwnerAbbreviations.append(vecAbbreviations[i]->text());
        }

        // Save
        DraftSettings::Get() = draftSettings;
        accept();
    });

    // Configure this dialog
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Adjust Settings"));
    setLayout(mainLayout);
    setModal(true);
}

