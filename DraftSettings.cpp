#include <assert.h>

#include "DraftSettings.h"

QString s_unknownString = "--";
QString s_errorString = "??";

DraftSettings& DraftSettings::Get()
{
    static DraftSettings s_draftSettings;
    return s_draftSettings;
}

DraftSettings::DraftSettings()
{
    m_ownerCount = 12;

    m_ownerNames = QStringList {
        "The 700 Level",
        "Hoi Pollois",
        "Wooden Spooners",
        "Gopher Balls",
        "Kilroy Sureshots",
        "Warning Track Power",
        "Alien Nation",
        "Phil D Walletup",
        "Has Bens",
        "Young Guns",
        "Master Batters",
        "Steroid Stiffs",
    };

    m_ownerAbbreviations = QStringList{
        "700",
        "HP",
        "WS",
        "GB",
        "KS",
        "WTP",
        "AN",
        "PDW",
        "HB",
        "YG",
        "MB",
        "SS",
    };

    assert(m_ownerNames.size() == m_ownerCount);
    assert(m_ownerAbbreviations.size() == m_ownerCount);

    // Taken from
    // http://razzball.com/10-14-16-mixed-league-and-nl-only-al-only-team-averages/
    m_statHistory[size_t(StatCategory::RUNS)] = StatTuple(791, 875, 706);
    m_statHistory[size_t(StatCategory::HOME_RUNS)] = StatTuple(184, 214, 153);
    m_statHistory[size_t(StatCategory::RBIS)] = StatTuple(775, 863, 688);
    m_statHistory[size_t(StatCategory::STEALS)] = StatTuple(111, 151, 71);
    m_statHistory[size_t(StatCategory::AVERAGE)] = StatTuple(0.271f, 0.283f, 0.259f);
    m_statHistory[size_t(StatCategory::WINS)] = StatTuple(87, 105, 69);
    m_statHistory[size_t(StatCategory::SAVES)] = StatTuple(38, 84, 12);
    m_statHistory[size_t(StatCategory::ERA)] = StatTuple(3.89f, 3.28f, 4.49f);
    m_statHistory[size_t(StatCategory::WHIP)] = StatTuple(1.29f, 1.22f, 1.35f);
    m_statHistory[size_t(StatCategory::STRIKEOUTS)] = StatTuple(1169, 1367, 971);
}

const uint32_t DraftSettings::OwnerCount()
{
    return Get().m_ownerCount;
}

const QString& DraftSettings::OwnerName(uint32_t i)
{
    if (i == 0) {
        return s_unknownString;
    } else if (i > Get().m_ownerCount) {
        return s_errorString;
    }
    
    return Get().m_ownerNames[i-1];
}

const QStringList& DraftSettings::OwnerNames()
{
    return Get().m_ownerNames;
}

const QString& DraftSettings::OwnerAbbreviation(uint32_t i)
{
    if (i == 0) {
        return s_unknownString;
    } else if (i > Get().m_ownerCount) {
        return s_errorString;
    }

    return Get().m_ownerAbbreviations[i-1];
}

const StatTuple& DraftSettings::StatHistoryCategory(const StatCategory& category)
{
    return Get().m_statHistory[size_t(category)];
}
