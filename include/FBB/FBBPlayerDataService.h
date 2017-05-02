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
using FBBPlayerID = QString;

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
    static FBBPlayer* GetPlayer(const FBBPlayerID& playerId);
    static FBBPlayer* GetPlayerFromBaseballReference(const QString& name, const QString& team);
    static void ForEach(const std::function<void(FBBPlayer&)>&& fn);
    static void ForEachValidHitter(const std::function<void(FBBPlayer&)>&& fn);
    static void ForEachValidPitcher(const std::function<void(FBBPlayer&)>&& fn);

private:

    FBBPlayerDataService(QObject* parent);
    std::vector<std::shared_ptr<FBBPlayer>> m_flatData;
    QMap<FBBPlayerID, std::shared_ptr<FBBPlayer>> m_mappedData;
};