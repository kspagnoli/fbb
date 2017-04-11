#pragma once

#include <QString>
#include <QMap>
#include <QTime>

#include <memory>

using FBBOwnerId = uint64_t;

struct FBBLeaugeSettings 
{
    struct Leauge
    {
        enum class Type
        {
            Mixed,
            NL,
            AL,

        } type = Type::Mixed;

        uint32_t budget = 260;
        uint32_t positionEligibility = 20;

    } leauge;

    struct Categories
    {
        struct Hitting
        {
            bool AVG = true;
            bool RBI = true;
            bool R = true;
            bool SB = true;
            bool HR = true;

        } hitting;

        struct Pitching
        {
            bool W = true;
            bool SV = true;
            bool ERA = true;
            bool WHIP = true;
            bool SO = true;

        } pitching;

    } categories;

    struct Positions
    {
        struct Hitting 
        {
            uint32_t numC = 2;
            uint32_t num1B = 1;
            uint32_t num2B = 1;
            uint32_t numSS = 1;
            uint32_t num3B = 1;
            uint32_t numMI = 1;
            uint32_t numCI = 1;
            uint32_t numIF = 0;
            uint32_t numLF = 0;
            uint32_t numCF = 0;
            uint32_t numRF = 0;
            uint32_t numOF = 5;
            uint32_t numDH = 0;
            uint32_t numUT = 1;

        } hitting;

        struct Pitching
        {
            uint32_t numSP = 0;
            uint32_t numRP = 0;
            uint32_t numP = 10;

        } pitching;

        uint32_t numBench = 3;

    } positions;

    struct Owner
    {
        QString name;
        QString abbreviation;
    };

    QMap<FBBOwnerId, QSharedPointer<Owner>> owners;

    struct Projections
    {
        enum class Source
        {
            ZiPS,
            Fans,
            Steamer,
            DepthCharts,
            ATC,
        };
        
        Source source = Source::Steamer;
        float hittingPitchingSplit = 0.70f;

    } projections;

    void CreateOwner()
    {
        FBBOwnerId ownerId = owners.empty() ? 100 : owners.lastKey() + QTime::currentTime().msecsSinceStartOfDay();
        owners.insert(ownerId, QSharedPointer<Owner>::create());
    }

    uint32_t SumHitters() const
    {
        return positions.hitting.numC +
            positions.hitting.num1B +
            positions.hitting.num2B +
            positions.hitting.numSS +
            positions.hitting.num3B +
            positions.hitting.numMI +
            positions.hitting.numCI +
            positions.hitting.numIF +
            positions.hitting.numLF +
            positions.hitting.numCF +
            positions.hitting.numRF +
            positions.hitting.numOF +
            positions.hitting.numDH +
            positions.hitting.numUT;
    }
    
    uint32_t SumPitchers() const
    {
        return positions.pitching.numP + 
            positions.pitching.numSP + 
            positions.pitching.numRP;
    }
    
    uint32_t SumPlayers() const
    {
        return SumHitters() + SumPitchers() + positions.numBench;
    }
};
