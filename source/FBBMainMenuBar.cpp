#include "FBB/FBBMainMenuBar.h"
#include "FBB/FBBLeaugeSettingsDialog.h"
#include "FBB/FBBLeaugeSettings.h"
#include "FBB/FBBPlayerDataService.h"
#include "FBB/FBBApplication.h"

#include <QMenu>
#include <QAction>
#include <QApplication>

FBBMainMenuBar::FBBMainMenuBar(QWidget* parent)
    : QMenuBar(parent)
{
    // File menu
    QMenu* fileMenu = addMenu("&File");
    fileMenu->addAction("&Exit", [=](){
        fbbApp->Exit();
    });

    fileMenu->addAction("&Save", [=](){
        // fbbApp->Save();
    });

    fileMenu->addAction("Save &As", [=](){
        fbbApp->SaveAs();
    });

    fileMenu->addAction("&Open", [=](){
        // fbbApp->Open();
    });

    // Settings
    QMenu* settingsMenu = addMenu("&Settings");

    // Settings > Settings
    QAction* pSettingsAction = new QAction("&Settings...", this);
    connect(pSettingsAction, &QAction::triggered, [=](bool checked) {
        FBBLeaugeSettings& settings = FBBLeaugeSettings::Instance();
        FBBLeaugeSettingsDialog dialog(&settings);
        if (dialog.exec()) {
            settings.OnAccept();
        }
    });
    settingsMenu->addAction(pSettingsAction);

    // Settings > Demo Data
    QAction* pDemoDataAction = new QAction("&Demo Data", this);
    connect(pDemoDataAction, &QAction::triggered, [=](bool checked) {
        FBBPlayerDataService::AddDemoData();
    });
    settingsMenu->addAction(pDemoDataAction);

}