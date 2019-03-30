#pragma once

#include <QString>
#include <QMap>
#include <QTime>
#include <QObject>

#include <memory>
#include <map>

using FBBOwnerId = uint64_t;

class FBBLeaugeSettings : public QObject
{
    Q_OBJECT

public:

    static FBBLeaugeSettings& Instance();

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

            bool OBP = false;
            bool SLG = false;
            bool OPS = false;
            bool H = false;
            bool TB = false;
            bool BB = false;
            bool RBIpR = false;
            bool xBH = false;
            bool SBmCS = false;
            bool wOBA = false;

        } hitting;

        struct Pitching
        {
            bool W = true;
            bool SV = true;
            bool ERA = true;
            bool WHIP = true;
            bool SO = true;

            bool AVG = false;
            bool Kp9 = false;
            bool KpBB = false;
            bool FIP = false;
            bool HLD = false;
            bool QS = false;

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
            uint32_t numU = 1;

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

    std::map<FBBOwnerId, std::shared_ptr<Owner>> owners;

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
        uint32_t minAB = 50;
        uint32_t minIP = 10;
        bool includeFA = false;

    } projections;

    std::shared_ptr<Owner> CreateOwner(const Owner& owner = Owner{})
    {
        FBBOwnerId ownerId = owners.empty() ? 100 : owners.rbegin()->first + QTime::currentTime().msecsSinceStartOfDay();
        std::shared_ptr<Owner> spOwner = std::make_shared<Owner>(owner);
        owners.insert(std::make_pair(ownerId, spOwner));
        return spOwner;
    }

    size_t SumHitters() const
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
            positions.hitting.numU;
    }
    
    size_t SumPitchers() const
    {
        return positions.pitching.numP + 
            positions.pitching.numSP + 
            positions.pitching.numRP;
    }
    
    size_t SumPlayers() const
    {
        return SumHitters() + SumPitchers() + positions.numBench;
    }
    
    void OnAccept() const
    {
        emit SettingsChanged(*this);
    }

signals:

    void SettingsChanged(const FBBLeaugeSettings& settings) const;

private:

    // Non-construct/copyable
    FBBLeaugeSettings(QObject* parent);
};