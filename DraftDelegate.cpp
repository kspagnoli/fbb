#include "DraftDelegate.h"
#include "Player.h"

#include <QApplication>
#include <QSortFilterProxyModel>

DraftDelegate::DraftDelegate(QWidget* parent)
    : QAbstractItemDelegate(parent)
{
}

void DraftDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Player::Status status = Player::Status(index.data().toInt());

    option.state;

    // Player is available
    if (status != Player::Status::Drafted) {

        QStyleOptionButton pushButtonOption;
        pushButtonOption.rect = option.rect;
        pushButtonOption.text = "Draft";
        pushButtonOption.state = option.state != QStyle::State_Selected ? QStyle::State_Raised : QStyle::State_Sunken; // [XXX] does nothing!

        QApplication::style()->drawControl(QStyle::CE_PushButton, &pushButtonOption, painter);
    }
}

QSize DraftDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QSize(option.rect.width(), option.rect.height());
}

bool DraftDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    // Skip if already drafted
    if (Player::Status(index.data().toInt()) == Player::Status::Drafted) {
        return false;
    }

    // Otherwise create a draft dialog
    if (event->type() == QEvent::MouseButtonRelease) {

        // This was probably from a proxy model
        QSortFilterProxyModel* proxyModel = dynamic_cast<QSortFilterProxyModel*>(model);
        QAbstractItemModel* srcModel = proxyModel ? proxyModel->sourceModel() : model;
        QModelIndex srcIndex = proxyModel ? proxyModel->mapToSource(index) : index;

        // Popup
        DraftDialog* draftDialog = new DraftDialog(srcModel, srcIndex);
        draftDialog->show();
        draftDialog->raise();
        draftDialog->activateWindow();

        // On successful draft, signal results
        connect(draftDialog, &QDialog::accepted, [=]() -> void {
            const DraftDialog::Results& results = draftDialog->GetDraftResults();
            emit Drafted(results, srcIndex, srcModel);
        });

        return true;
    }

    return false;
}

#include "DraftDelegate.moc"
