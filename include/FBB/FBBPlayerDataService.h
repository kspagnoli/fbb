#pragma once

#include "FBB/FBBLeaugeSettings.h"

#include <QObject>
#include <QList>
#include <QString>
#include <QMap>

#include <vector>
#include <memory>
#include <functional>

class FBBPlayer;
using FBBPlayerId = QString;

//------------------------------------------------------------------------------
// FBBPlayerDataService
//------------------------------------------------------------------------------
class FBBPlayerDataService : public QObject
{
    Q_OBJECT;

public:

    static FBBPlayerDataService& Instance();

    static uint32_t PlayerCount();
    static FBBPlayer* GetPlayer(uint32_t index);
    static FBBPlayer* GetPlayer(const FBBPlayerId& playerId);
    static FBBPlayer* GetPlayerFromBaseballReference(const QString& name, const QString& team);
    static void ForEach(const std::function<void(FBBPlayer&)>&& fn);
    static std::vector<FBBPlayer*> GetValidHitters();
    static std::vector<FBBPlayer*> GetValidPitchers();

    static bool IsValidUnderCurrentSettings(const FBBPlayer* player);

private:

    FBBPlayerDataService(QObject* parent);
    std::vector<std::shared_ptr<FBBPlayer>> m_flatData;
    QMap<FBBPlayerId, std::shared_ptr<FBBPlayer>> m_mappedData;
};