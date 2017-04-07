#pragma once

#include <QString>
#include <QHash>
#include <memory>

struct FreeAgent
{
    QString player;
    QString owner;
    bool position_C;
    bool position_1B;
    bool position_2B;
    bool position_3B;
    bool position_SS;
    bool position_OF;
};

class FreeAgents
{
public:
    static const std::shared_ptr<FreeAgent>& Lookup(const QString playerId);

private:

    FreeAgents();
    QHash<QString, std::shared_ptr<FreeAgent>> m_mapFreeAgents;
};