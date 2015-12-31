#include "DraftDialog.h"
#include "PlayerTableModel.h"

#include <QGridLayout>
#include <QString>
#include <QComboBox>
#include <QStringList>
#include <QLabel>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QPushButton>
#include <QLineEdit>
#include <QSortFilterProxyModel>

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

DraftDialog::DraftDialog(QAbstractItemModel* model, const QModelIndex& index)
{
    // Cancel button
    QPushButton* cancelButton = new QPushButton("Cancel");
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Draft button
    QPushButton* draftButton = new QPushButton("Draft");
    draftButton->setDefault(true);
    draftButton->setDisabled(true);
    connect(draftButton, &QPushButton::clicked, this, &QDialog::accept);

    // Button layout
    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(draftButton);

    // Info grid to hold top data
    QGridLayout* infoGridLayout = new QGridLayout;

    // Player name
    {
        QModelIndex playerNameIndex = model->index(index.row(), PlayerTableModel::COLUMN_NAME);
        QString playerName = model->data(playerNameIndex).toString();
        infoGridLayout->addWidget(new QLabel("Player Name:"), PlayerNameRow, LabelColumn);
        infoGridLayout->addWidget(new QLabel(playerName), PlayerNameRow, ItemColumn);
    }

    // Owner combo box
    {
        QComboBox* ownerComboBox = new QComboBox;
        QStringList list ={ "Select...", "Team A", "Team B" };
        ownerComboBox->addItems(list);
        infoGridLayout->addWidget(new QLabel("Owner:"), OwnerRow, LabelColumn);
        infoGridLayout->addWidget(ownerComboBox, OwnerRow, ItemColumn);
        connect(ownerComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, [=](int index) {
            m_draftResults.ownerId = index;
            draftButton->setDisabled(!m_draftResults.cost || !m_draftResults.ownerId);
        });
    }

    // Cost line edit
    {
        const uint32_t MAX_BID = 250;
        QLineEdit* costLineEdit = new QLineEdit;
        costLineEdit->setValidator(new QIntValidator(0, MAX_BID, this));
        infoGridLayout->addWidget(new QLabel("Cost:"), CostRow, LabelColumn);
        infoGridLayout->addWidget(costLineEdit, CostRow, ItemColumn);
        connect(costLineEdit, &QLineEdit::textEdited, this, [=](const QString& text) {
            m_draftResults.cost = text.toInt();
            draftButton->setDisabled(!m_draftResults.cost || !m_draftResults.ownerId);
        });
    }

    // Position combo box
    {
        QComboBox* positionComboBox = new QComboBox;
        QModelIndex playerPositionsIndex = model->index(index.row(), PlayerTableModel::COLUMN_POSITION);
        uint32_t playerPositions = model->data(playerPositionsIndex, PlayerTableModel::RawDataRole).toUInt();
        QStringList positionStrings = PositionToStringList(playerPositions);
        positionComboBox->addItems(positionStrings);
        infoGridLayout->addWidget(new QLabel("Position:"), PositionRow, LabelColumn);
        infoGridLayout->addWidget(positionComboBox, PositionRow, ItemColumn);
        connect(positionComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::activated), this, [=](const QString& position) {
            m_draftResults.position = position;
        });

        // Set default position to first in the list
        m_draftResults.position = positionStrings.first();
    }

    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(infoGridLayout);
    mainLayout->addSpacing(6);
    mainLayout->addLayout(buttonsLayout);

    // Configure this dialog
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Draft Player..."));
    setLayout(mainLayout);
    setModal(true);
}

const DraftDialog::Results& DraftDialog::GetDraftResults() const
{
    return m_draftResults;
}