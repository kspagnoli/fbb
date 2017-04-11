#pragma once

#include <QDialog>

struct FBBLeaugeSettings;

class FBBLeaugeSettingsDialog : public QDialog
{
public:
    FBBLeaugeSettingsDialog(FBBLeaugeSettings* pSettings, QWidget* pParent = nullptr);
};