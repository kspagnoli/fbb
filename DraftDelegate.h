#pragma once

#include "DraftDialog.h"
#include <QAbstractItemDelegate>

class PlayerTableModel;

class DraftDelegate : public QAbstractItemDelegate
{
public:

    DraftDelegate(PlayerTableModel* playerModel);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

private:
    PlayerTableModel* m_playerTableModel;
};
