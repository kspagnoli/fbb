#include "FBB/FBBMainMenuBar.h"
#include "FBB/FBBLeaugeSettingsDialog.h"
#include "FBB/FBBLeaugeSettings.h"

#include <QMenu>
#include <QAction>

FBBMainMenuBar::FBBMainMenuBar(QWidget* parent)
    : QMenuBar(parent)
{
    // File menu
    QMenu* fileMenu = addMenu("&File");

    // Settings
    QMenu* settingsMenu = addMenu("&Settings");

    // Settings > Settings
    QAction* settingsAction = new QAction("&Settings...", this);
    connect(settingsAction, &QAction::triggered, [=](bool checked) {
        FBBLeaugeSettings settings;
        FBBLeaugeSettingsDialog dialog(&settings);
        if (dialog.exec()) {
        }
    });
    settingsMenu->addAction(settingsAction);
}