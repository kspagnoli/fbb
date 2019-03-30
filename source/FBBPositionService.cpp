#include "FBB/FBBPositionService.h"
#include "FBB/FBBPlayerDataService.h"
#include "FBB/FBBPlayer.h"
#include "FBB/FBBLeaugeSettings.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QMap>

FBBPositionService::FBBPositionService(QObject* parent)
{
    // Listen for settings changes
    connect(&FBBLeaugeSettings::Instance(), &FBBLeaugeSettings::SettingsChanged, this, [=] {
        LoadPositionData();
    });
}

void FBBPositionService::LoadPositionData()
{
}

FBBPositionService& FBBPositionService::Instance()
{
    static FBBPositionService* s_service = new FBBPositionService(qApp);
    return *s_service;
}
