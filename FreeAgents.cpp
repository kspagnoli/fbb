#include "FreeAgents.h"
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

FreeAgents::FreeAgents()
{
    // Log
    GlobalLogger::AppendMessage("Loading free agents...");

    // Open file
    QFile inputFile(":/data/2017_FAHitters.csv");
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

            const std::string& team        = parsed.at(0);
            const std::string& player      = parsed.at(1);
            const std::string& owner       = parsed.at(2);
            const std::string& position_C  = parsed.at(3);
            const std::string& position_1B = parsed.at(4);
            const std::string& position_2B = parsed.at(5);
            const std::string& position_3B = parsed.at(6);
            const std::string& position_SS = parsed.at(7);
            const std::string& position_OF = parsed.at(8);

            // XXX
            std::string id = "s_" + player + "_" + team;

            // Build news struct
            std::shared_ptr<FreeAgent> spFreeAgent = std::make_shared<FreeAgent>();
            
            // Save in database
            m_mapFreeAgents[QString::fromStdString(id)] = std::move(spFreeAgent);

        } catch (...) {

            // Try next if something went wrong
            GlobalLogger::AppendMessage(QString("Failed to parse news for a player"));
            continue;
        }
    }
}

const std::shared_ptr<FreeAgent>& FreeAgents::Lookup(const QString playerId)
{
    static FreeAgents s_instance;
    return s_instance.m_mapFreeAgents[playerId];
}
