#pragma once

#include "FBB/FBBLeaugeSettings.h"

#include <QObject>

//------------------------------------------------------------------------------
// FBBProjectionService
//------------------------------------------------------------------------------
class FBBProjectionService : public QObject
{
    Q_OBJECT;

public:

    static FBBProjectionService& Instance();

signals:
    void BeginProjectionsUpdated();
    void EndProjectionsUpdated();

private:

    FBBProjectionService(QObject* parent);
    void LoadProjections(const FBBLeaugeSettings::Projections::Source& source);
};