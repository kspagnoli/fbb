#pragma once

#include <QDialog>

class FBBPlayer;

class FBBDraftDialog : public QDialog
{
    Q_OBJECT

public:
    FBBDraftDialog(FBBPlayer* pPlayer);

private:

    // Values we are filling in
    int m_paid = -1;
    QString m_owner;
    int m_position = -1;
};