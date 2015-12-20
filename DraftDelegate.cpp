#include "DraftDelegate.h"
#include "Player.h"

#include <QApplication>

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

        DraftDialog* draftDialog = new DraftDialog(model, index);
        draftDialog->show();
        draftDialog->raise();
        draftDialog->activateWindow();

        // On successful draft, signal results
        connect(draftDialog, &QDialog::accepted, [=]() -> void {
            const DraftDialog::Results& results = draftDialog->GetDraftResults();
            emit Drafted(results, index, model);
        });

        return true;
    }

    return false;
}

#include "DraftDelegate.moc"
