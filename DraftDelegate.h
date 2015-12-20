#pragma once

#include "DraftDialog.h"
#include <QAbstractItemDelegate>

class DraftDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:

    DraftDelegate(QWidget* parent);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

signals:

    void Drafted(const DraftDialog::Results& results, const QModelIndex& index, QAbstractItemModel* model);
};