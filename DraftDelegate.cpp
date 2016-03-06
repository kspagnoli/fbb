#include "DraftDelegate.h"
#include "Player.h"
#include "PlayerTableModel.h"

#include <QApplication>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QPainter>

DraftDelegate::DraftDelegate(PlayerTableModel* playerTableModel)
    : QAbstractItemDelegate(playerTableModel)
    , m_playerTableModel(playerTableModel)
{
}

void DraftDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // Get Player::ownerId
    auto proxyModel = dynamic_cast<const QSortFilterProxyModel*>(index.model());
    auto srcIdx = proxyModel->mapToSource(index);
    auto ownerIdx = m_playerTableModel->index(srcIdx.row(), PlayerTableModel::COLUMN_OWNER);
    auto ownerId = m_playerTableModel->data(ownerIdx, PlayerTableModel::RawDataRole);

    QStyleOptionButton pushButtonOption;
    pushButtonOption.rect = option.rect;
    pushButtonOption.text = (ownerId == 0) ? "Draft" : "Return";
    pushButtonOption.state = QStyle::State_Mini;

    auto opacity = painter->opacity();
    painter->setOpacity(ownerId == 0 ? 1.0f : 0.3f);
    QApplication::style()->drawControl(QStyle::CE_PushButton, &pushButtonOption, painter);
    painter->setOpacity(opacity);
}

QSize DraftDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(option.rect.width(), option.rect.height());
}

bool DraftDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    // On mouse release...
    if (event->type() == QEvent::MouseButtonRelease) {

        // This was probably from a proxy model
        // XXX: i think this can go away...
        auto proxyModel = dynamic_cast<const QSortFilterProxyModel*>(index.model());
        QAbstractItemModel* srcModel = proxyModel ? proxyModel->sourceModel() : model;
        QModelIndex srcIndex = proxyModel ? proxyModel->mapToSource(index) : index;

        // Get Player::ownerId
        auto srcIdx = proxyModel->mapToSource(index);
        auto ownerIdx = m_playerTableModel->index(srcIdx.row(), PlayerTableModel::COLUMN_OWNER);
        auto ownerId = m_playerTableModel->data(ownerIdx, PlayerTableModel::RawDataRole).toUInt();

        // If already owned...
        if (ownerId != 0) {

            // Get Player::ownerId
            auto playerNameIdx = m_playerTableModel->index(srcIdx.row(), PlayerTableModel::COLUMN_NAME);
            auto playerName = m_playerTableModel->data(playerNameIdx, PlayerTableModel::RawDataRole).toString();

            // Create a warning popup 
            QMessageBox warning;
            warning.setText(QString("Do you want to return %1 to the available player pool?").arg(playerName));
            warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            warning.setDefaultButton(QMessageBox::No);
            if (warning.exec() == QMessageBox::Yes) {
                DraftDialog::Results results;
                results.previousOwnerId = ownerId;
                m_playerTableModel->OnDrafted(results, srcIndex, srcModel);
                return true;
            }

        } else {

            // Create a draft dialog popup
            DraftDialog* draftDialog = new DraftDialog(srcModel, srcIndex);
            if (draftDialog->exec() == QDialog::Accepted) {
                const DraftDialog::Results& results = draftDialog->GetDraftResults();
                m_playerTableModel->OnDrafted(results, srcIndex, srcModel);
                return true;
            }
        }
    }

    return false;
}
