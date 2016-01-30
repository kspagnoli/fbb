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

namespace DraftSettings
{
    const uint32_t Budget();
    const uint32_t HitterCount();
    const uint32_t PitcherCount();
    const uint32_t RosterSize();

    const uint32_t OwnerCount();
    const QString& OwnerName(uint32_t i);
    const QStringList& OwnerNames();
    const QString& OwnerAbbreviation(uint32_t i);
    const StatTuple& StatHistoryCategory(const StatCategory& category);
};