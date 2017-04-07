#include "PlayerNews.h"
#include "GlobalLogger.h"

#include <vector>
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <QFile>
#include <QString>
#include <QTextStream>

PlayerNews::PlayerNews()
{
    // Log
    GlobalLogger::AppendMessage("Loading player news...");

    // Open file
    QFile inputFile(":/data/News_2017.csv");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream file(&inputFile);

    // CSV tokenizer
    using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;

    // Loop rows
    while (!file.atEnd()) {

        // Tokenization and/or lexical casts might fail if the data is malformed
        try {

            // Tokenize this row
            std::string row = file.readLine().toStdString();
            Tokenizer tokenizer(row);
            std::vector<std::string> parsed(tokenizer.begin(), tokenizer.end());

            // Parse name
            std::string name = parsed.at(0);
            std::string id = parsed.at(1);
            std::string details = parsed.at(2);
            std::string quick = parsed.at(3);
            std::string news = parsed.at(4);

            // Build news struct
            std::shared_ptr<News> spNews = std::make_shared<News>();
            spNews->details = QString::fromStdString(details);
            spNews->quick = QString::fromStdString(quick);
            spNews->news = QString::fromStdString(news);

           // Save in database
           m_mapNews[QString::fromStdString(id)] = std::move(spNews);

        } catch (...) {

            // Try next if something went wrong
            GlobalLogger::AppendMessage(QString("Failed to parse news for a player"));
            continue;
        }
    }
}

const std::shared_ptr<News>& PlayerNews::Lookup(const QString playerId) 
{
    static PlayerNews s_instance;
    return s_instance.m_mapNews[playerId];
}
