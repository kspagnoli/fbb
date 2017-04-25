#pragma once

#include <QDialog>

class FBBLeaugeSettings;

class FBBLeaugeSettingsDialog : public QDialog
{
public:
    FBBLeaugeSettingsDialog(FBBLeaugeSettings* pSettings, QWidget* pParent = nullptr);
};