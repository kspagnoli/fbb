#pragma once

#include "FBB/FBBLeaugeSettings.h"

#include <QObject>
#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QMap>

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
    static const QSharedPointer<FBBPlayer> GetPlayer(uint32_t index);
    static const QSharedPointer<FBBPlayer> GetPlayer(const FBBPlayerID& playerId);
    static void ForEach(const std::function<void(FBBPlayer&)>&& fn);

private:

    FBBPlayerDataService(QObject* parent);
    QList<QSharedPointer<FBBPlayer>> m_flatData;
    QMap<FBBPlayerID, QSharedPointer<FBBPlayer>> m_mappedData;
};