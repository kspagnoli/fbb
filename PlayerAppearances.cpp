#define _SCL_SECURE_NO_WARNINGS

#include "PlayerAppearances.h"
#include "GlobalLogger.h"

#include <vector>
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>

#include <QFile>
#include <QString>
#include <QTextStream>

PlayerApperances::PlayerApperances()
{
    // Log
    GlobalLogger::AppendMessage("Loading player appearances...");

    // Open file
    QFile inputFile(":/data/2016_Appearances.csv");
    inputFile.open(QIODevice::ReadOnly);
    QTextStream file(&inputFile);

    // Tokenize header data
    using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;
    std::string row = file.readLine().toStdString();
    Tokenizer tokenizer(row);


    // Loop rows
    // while (std::getline(apperances, row)) {
    while (!file.atEnd()) {

        // Tokenization and/or lexical casts might fail if the data is malformed
        try {

            // Tokenize this row
            std::string row = file.readLine().toStdString();

            // Baseball reference likes \ to delimit as well?
            boost::replace_all(row, "\\", ",");

            // Tokenize!
            Tokenizer tokenizer(row);
            std::vector<std::string> parsed(tokenizer.begin(), tokenizer.end());

            // Parse name
            std::string name = parsed[1];
            std::string id   = parsed[2];
            std::string team = parsed[4];

            // Parse apperances
            Appearances apperances;
            apperances.age  = boost::lexical_cast<uint32_t>(parsed[3]);
            if (parsed[5] == "1st") {
                apperances.exp = 0;
            } else {
                apperances.exp = boost::lexical_cast<uint32_t>(parsed[5]);
            }
            apperances.G    = boost::lexical_cast<uint32_t>(parsed[6]);
            apperances.GS   = boost::lexical_cast<uint32_t>(parsed[7]);
            apperances.atC  = boost::lexical_cast<uint32_t>(parsed[11]);
            apperances.at1B = boost::lexical_cast<uint32_t>(parsed[12]);
            apperances.at2B = boost::lexical_cast<uint32_t>(parsed[13]);
            apperances.at3B = boost::lexical_cast<uint32_t>(parsed[14]);
            apperances.atSS = boost::lexical_cast<uint32_t>(parsed[15]);
            apperances.atOF = boost::lexical_cast<uint32_t>(parsed[19]);
            apperances.atDH = boost::lexical_cast<uint32_t>(parsed[20]);

            // Get key
            auto key = std::make_tuple(name, team);
            if (m_mapAppearances.find(key) != m_mapAppearances.end()) {
                throw 0;
            } 

            // Save in database
            m_mapAppearances.emplace(std::move(std::make_pair(key, apperances)));

        } catch (...) {

            // Try next if something went wrong
            GlobalLogger::AppendMessage(QString("Failed to parse appearance records for a player"));
            continue;
        }
    }
}

const Appearances& PlayerApperances::Lookup(const std::string& name) const
{
    auto itr = std::find_if(std::begin(m_mapAppearances), std::end(m_mapAppearances), [=](auto& player) {
        return std::get<0>(player.first) == name;
    });

    if (itr == m_mapAppearances.end()) {
        std::stringstream ss;
        ss << "No appearance entry for: " << name;
        throw std::runtime_error(ss.str());
    }

    return itr->second;
}