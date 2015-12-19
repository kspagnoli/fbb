#pragma once

#include <QDialog>

class QAbstractItemModel;
class QModelIndex;

class DraftDialog : public QDialog
{
public:
    DraftDialog(QAbstractItemModel* model, const QModelIndex& index);
};