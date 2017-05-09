#include "FBB/FBBDraftDialog.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBPosition.h"
#include "FBB/FBBLeaugeSettings.h"

#include <QPushButton>
#include <QBoxLayout>
#include <QGridLayout>
#include <QString>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>

enum
{
    PlayerNameRow,
    OwnerRow,
    CostRow,
    PositionRow,
};

enum
{
    LabelColumn,
    ItemColumn,
};

FBBDraftDialog::FBBDraftDialog(FBBPlayer* pPlayer)
{
    // Cancel button
    QPushButton* pCancelButton = new QPushButton("Cancel");

    // Draft button
    QPushButton* pDraftButton = new QPushButton("Draft");
    pDraftButton->setDefault(true);
    pDraftButton->setDisabled(true);
    
    // Button layout
    QHBoxLayout* pButtonsLayout = new QHBoxLayout;
    pButtonsLayout->addWidget(pCancelButton);
    pButtonsLayout->addWidget(pDraftButton);

    // Info grid to hold top data
    QGridLayout* pInfoGridLayout = new QGridLayout;

    // Player name
    pInfoGridLayout->addWidget(new QLabel("Player Name:"), PlayerNameRow, LabelColumn);
    pInfoGridLayout->addWidget(new QLabel(pPlayer->name), PlayerNameRow, ItemColumn);

    // Owner combo box
    const QString defaultOwnerText = "Select...";
    QComboBox* pOwnerComboBox = new QComboBox;
    QStringList list = { defaultOwnerText };
    for (const auto& item : FBBLeaugeSettings::Instance().owners) {
        list += item.second->name;
    }
    pOwnerComboBox->addItems(list);
    pInfoGridLayout->addWidget(new QLabel("Owner:"), OwnerRow, LabelColumn);
    pInfoGridLayout->addWidget(pOwnerComboBox, OwnerRow, ItemColumn);

    // Cost line edit
    const uint32_t MAX_BID = FBBLeaugeSettings::Instance().leauge.budget;
    QLineEdit* pCostLineEdit = new QLineEdit;
    pCostLineEdit->setValidator(new QIntValidator(0, MAX_BID, this));
    pInfoGridLayout->addWidget(new QLabel("Cost:"), CostRow, LabelColumn);
    pInfoGridLayout->addWidget(pCostLineEdit, CostRow, ItemColumn);

    // Position combo box
    QComboBox* pPositionComboBox = new QComboBox;
    QStringList positionStrings = FBBPositionMaskToStringList(pPlayer->eligablePositions);
    pPositionComboBox->addItems(positionStrings);
    pInfoGridLayout->addWidget(new QLabel("Position:"), PositionRow, LabelColumn);
    pInfoGridLayout->addWidget(pPositionComboBox, PositionRow, ItemColumn);

    // Set default position to first in the list
    /// unsigned long bit = 0;
    /// _BitScanForward(&bit, playerPositionBitField);
    /// m_draftResults.position = PlayerPosition(bit);

    // Main layout
    QVBoxLayout* pMainLayout = new QVBoxLayout;
    pMainLayout->addLayout(pInfoGridLayout);
    pMainLayout->addSpacing(6);
    pMainLayout->addLayout(pButtonsLayout);

    // Rejected
    connect(pCancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Accept
    connect(pDraftButton, &QPushButton::clicked, this, [=]() {
        
        // apply paid
        pPlayer->draftInfo.paid = m_paid;

        // apply owner
        for (const auto& item : FBBLeaugeSettings::Instance().owners) {
             if (item.second->name == m_owner) {
                 pPlayer->draftInfo.owner = item.first;
                 break;
            }
         }

        accept();
    });

    connect(pOwnerComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::activated), this, [=](const QString& selected) {
        if (m_owner != defaultOwnerText) {
            m_owner = selected;
        }
        pDraftButton->setEnabled(m_paid > 0 && !m_owner.isEmpty());
    });

    connect(pCostLineEdit, &QLineEdit::textEdited, this, [=](const QString& text) {
        m_paid = text.toInt();
        pDraftButton->setEnabled(m_paid > 0 && !m_owner.isEmpty());
    });

    connect(pPositionComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::activated), this, [=](const QString& text) {
        m_position = text.toInt();
    });

    // Configure this dialog
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Draft Player..."));
    setLayout(pMainLayout);
    setModal(true);
}