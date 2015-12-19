#pragma once

#include <QDialog>

class QAbstractItemModel;
class QModelIndex;



class DraftDialog : public QDialog
{
public:
    struct Results
    {
        uint32_t ownerId = 0;
        uint32_t cost = 0;
    };

    DraftDialog(QAbstractItemModel* model, const QModelIndex& index);
    const Results& GetDraftResults() const;

private:
    Results m_draftResults;
};