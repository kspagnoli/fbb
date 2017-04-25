#include "FBB/FBBLeaugeSettings.h"

#include <QApplication>

FBBLeaugeSettings::FBBLeaugeSettings(QObject* parent)
    : QObject(parent)
{
}

FBBLeaugeSettings& FBBLeaugeSettings::Instance()
{
    static FBBLeaugeSettings* s_instance = new FBBLeaugeSettings(qApp);
    return *s_instance;
}


#include "FBBLeaugeSettings.moc"