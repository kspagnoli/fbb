#pragma once

#include <QString>
#include <QHash>
#include <memory>

struct News
{
    QString details;
    QString quick;
    QString news;
};

class PlayerNews
{
public:
    static const std::shared_ptr<News>& Lookup(const QString playerId);

private:

    PlayerNews();
    QHash<QString, std::shared_ptr<News>> m_mapNews;
};