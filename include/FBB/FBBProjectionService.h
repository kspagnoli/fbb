#pragma once

#include "FBB/FBBLeaugeSettings.h"

#include <QObject>
#include <vector>

class FBBPlayer;

//------------------------------------------------------------------------------
// FBBProjectionService
//------------------------------------------------------------------------------
class FBBProjectionService : public QObject
{
    Q_OBJECT;

public:

    static FBBProjectionService& Instance();
    FBBProjectionService(QObject* parent);
    void LoadProjections();

private:
    void LoadPitchingProjections(std::vector<FBBPlayer*>& vecPlayers, const QString& file);
    void LoadHittingProjections(std::vector<FBBPlayer*>& vecPlayers, const QString& file);
};