#pragma once

#include "FBB/FBBLeaugeSettings.h"

#include <QObject>

//------------------------------------------------------------------------------
// FBBPositionService
//------------------------------------------------------------------------------
class FBBPositionService : public QObject
{
    Q_OBJECT;

public:

    static void LoadPositionData();

private:

    static FBBPositionService& Instance();
    FBBPositionService(QObject* parent);
};