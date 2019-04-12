#include "FBB/FBBLeaugeSettings.h"

#include <QApplication>

FBBLeaugeSettings::FBBLeaugeSettings(QObject* parent)
    : QObject(parent)
{
    CreateOwner({ "Owner #1", "FOO" });
    CreateOwner({ "Owner #2", "BAR" });
    CreateOwner({ "Owner #3", "BIZ" });
    CreateOwner({ "Owner #4", "BIZ" });
    CreateOwner({ "Owner #5", "BIZ" });
    CreateOwner({ "Owner #6", "BIZ" });
    CreateOwner({ "Owner #7", "BIZ" });
    CreateOwner({ "Owner #8", "BIZ" });
    CreateOwner({ "Owner #9", "BIZ" });
    CreateOwner({ "Owner #10", "BIZ" });
    CreateOwner({ "Owner #11", "BIZ" });
    CreateOwner({ "Owner #12", "BIZ" });
}

#include "FBBLeaugeSettings.moc"
