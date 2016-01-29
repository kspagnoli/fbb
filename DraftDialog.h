#pragma once

#include <QDialog>
#include <QString>

#include "Player.h"

class QAbstractItemModel;
class QModelIndex;

class DraftDialog : public QDialog
{
public:

    struct Results
    {
        uint32_t ownerId = 0;
        uint32_t previousOwnerId = 0;
        uint32_t cost = 0;
        PlayerPosition position = PlayerPosition::None;
    };

    DraftDialog(QAbstractItemModel* model, const QModelIndex& index);
    const Results& GetDraftResults() const;

private:
    Results m_draftResults;
};