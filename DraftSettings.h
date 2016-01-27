#pragma once

#include <cstdint>
#include <QStringList>
#include <array>
#include <tuple>

enum class StatCategory
{
    RUNS,
    HOME_RUNS,
    RBIS,
    STEALS,
    AVERAGE,
    WINS,
    SAVES,
    ERA,
    WHIP,
    STRIKEOUTS,
    COUNT
};

using StatTuple = std::tuple<float, float, float>;

class DraftSettings
{
public:

    static const uint32_t OwnerCount();
    static const QString& OwnerName(uint32_t i);
    static const QStringList& OwnerNames();
    static const QString& OwnerAbbreviation(uint32_t i);
    static const StatTuple& StatHistoryCategory(const StatCategory& category);

private:

    DraftSettings();
    static DraftSettings& Get();

    uint32_t m_ownerCount;
    QStringList m_ownerNames;
    QStringList m_ownerAbbreviations;
    std::array<StatTuple, size_t(StatCategory::COUNT)> m_statHistory;
};