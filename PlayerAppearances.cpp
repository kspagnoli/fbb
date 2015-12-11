#include "PlayerAppearances.h"

#include <vector>
#include <fstream>
#include <bitset>
#include <unordered_map>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

PlayerApperances::PlayerApperances(const std::string& filename)
{
    // Open file
    std::fstream apperances(filename);
    std::string row;

    // Tokenize header data
    using Tokenizer = boost::tokenizer<boost::escaped_list_separator<char>>;
    std::getline(apperances, row);
    Tokenizer tokenizer(row);

    // Stats to find
    std::unordered_map<std::string, uint32_t> LUT =
    {
        { "Name",0 },
        { "Tm",0 },
        { "G",0 },
        { "GS",0 },
        { "P",0 },
        { "C",0 },
        { "1B",0 },
        { "2B",0 },
        { "3B",0 },
        { "SS",0 },
        { "OF",0 },
        { "DH",0 },
    };

    // Parse the header data (first row)
    for (auto& entry : LUT) {

        // Populate LUT to figure out stat-to-column association
        auto itr = std::find(tokenizer.begin(), tokenizer.end(), entry.first);
        if (itr != tokenizer.end()) {
            entry.second = std::distance(tokenizer.begin(), itr);
        }
    }

    // Loop rows
    while (std::getline(apperances, row)) {

        // Tokenization and/or lexical casts might fail if the data is malformed
        try {

            // Tokenize this row
            Tokenizer tokenizer(row);
            std::vector<std::string> parsed(tokenizer.begin(), tokenizer.end());

            // Parse name
            std::string name = parsed[LUT["Name"]];

            // Parse apperances
            Appearances apperances;
            apperances.G    = boost::lexical_cast<uint32_t>(parsed[LUT["G"]]);
            apperances.GS   = boost::lexical_cast<uint32_t>(parsed[LUT["GS"]]);
            apperances.atC  = boost::lexical_cast<uint32_t>(parsed[LUT["C"]]);
            apperances.at1B = boost::lexical_cast<uint32_t>(parsed[LUT["1B"]]);
            apperances.at2B = boost::lexical_cast<uint32_t>(parsed[LUT["2B"]]);
            apperances.at3B = boost::lexical_cast<uint32_t>(parsed[LUT["3B"]]);
            apperances.atSS = boost::lexical_cast<uint32_t>(parsed[LUT["SS"]]);
            apperances.atOF = boost::lexical_cast<uint32_t>(parsed[LUT["OF"]]);
            apperances.atDH = boost::lexical_cast<uint32_t>(parsed[LUT["DH"]]);

            // 
            auto key = std::make_tuple(name, "XXX");
            if (m_mapAppearances.find(key) != m_mapAppearances.end()) {
                throw 0;
            } 

            m_mapAppearances.emplace(std::move(std::make_pair(key, apperances)));

        } catch (...) {

            // Try next if something went wrong
            continue;
        }
    }
}

const Appearances& PlayerApperances::Lookup(const std::string& name) const
{
    auto key = std::make_tuple(name, "XXX");

    auto itr = m_mapAppearances.find(key);
    if (itr == m_mapAppearances.end()) {

        std::stringstream ss;
        ss << "No appearance entry for: " << name;

        throw std::runtime_error(ss.str());
    }

    return itr->second;
}