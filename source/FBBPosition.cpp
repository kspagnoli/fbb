#include "FBB/FBBPosition.h"
#include "FBB/FBBLeaugeSettings.h"
#include "FBB/FBBApplication.h"

#include <QStringList>

QString FBBPositionToString(FBBPositionBits bit)
{
    switch (bit)
    {
    case FBB_POSITION_UNKNOWN:
        return "??";
    case FBB_POSITION_C:
        return "C";
    case FBB_POSITION_1B:
        return "1B";
    case FBB_POSITION_2B:
        return "2B";
    case FBB_POSITION_SS:
        return "SS";
    case FBB_POSITION_3B:
        return "3B";
    case FBB_POSITION_RF:
        return "RF";
    case FBB_POSITION_CF:
        return "CF";
    case FBB_POSITION_LF:
        return "LF";
    case FBB_POSITION_DH:
        return "DH";
    case FBB_POSITION_SP:
        return "SP";
    case FBB_POSITION_RP:
        return "RP";
    case FBB_POSITION_CI:
        return "CI";
    case FBB_POSITION_MI:
        return "MI";
    case FBB_POSITION_IF:
        return "IF";
    case FBB_POSITION_OF:
        return "OF";
    case FBB_POSITION_U:
        return "U";
    case FBB_POSITION_P:
        return "P";
    default:
        break;
    }

    return "<ERR>";
}

QString FBBPositionMaskToString(FBBPositionMask mask, bool includeAggregates)
{
    return FBBPositionMaskToStringList(mask, includeAggregates).join("/");
}

QStringList FBBPositionMaskToStringList(FBBPositionMask mask, bool includeAggregates)
{
    // String list we will build up
    QStringList stringList;

    if (mask == FBB_POSITION_UNKNOWN)
    {
        stringList << FBBPositionToString(FBB_POSITION_UNKNOWN);
        return stringList;
    }

    // Hitting Positions
    if (mask & FBB_POSITION_C)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_C))
        {
            stringList << FBBPositionToString(FBB_POSITION_C);
        }
    }

    if (mask & FBB_POSITION_1B)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_1B))
        {
            stringList <<  FBBPositionToString(FBB_POSITION_1B);
        }
    }

    if (mask & FBB_POSITION_2B)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_2B))
        {
            stringList << FBBPositionToString(FBB_POSITION_2B);
        }
    }

    if (mask & FBB_POSITION_SS)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_SS))
        {
            stringList << FBBPositionToString(FBB_POSITION_SS);
        }
    }

    if (mask & FBB_POSITION_3B)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_3B))
        {
            stringList << FBBPositionToString(FBB_POSITION_3B);
        }
    }

    if (mask & FBB_POSITION_RF)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_RF))
        {
            stringList <<  FBBPositionToString(FBB_POSITION_RF);
        }

    }

    if (mask & FBB_POSITION_CF)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_CF))
        {
            stringList <<  FBBPositionToString(FBB_POSITION_CF);
        }

    }

    if (mask & FBB_POSITION_LF)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_LF))
        {
            stringList <<  FBBPositionToString(FBB_POSITION_LF);
        }

    }

    if (mask & FBB_POSITION_DH)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_DH))
        {
            stringList <<  FBBPositionToString(FBB_POSITION_DH);
        }
    }

    if (mask & FBB_POSITION_SP)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_SP))
        {
            stringList <<  FBBPositionToString(FBB_POSITION_SP);
        }
    }

    if (mask & FBB_POSITION_RP)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_RP))
        {
            stringList <<  FBBPositionToString(FBB_POSITION_RP);
        }
    }

    if (mask & FBB_POSITION_CI)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_CI))
        {
            stringList <<  FBBPositionToString(FBB_POSITION_CI);
        }
    }

    if (mask & FBB_POSITION_MI)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_MI))
        {
            stringList <<  FBBPositionToString(FBB_POSITION_MI);
        }
    }

    if (mask & FBB_POSITION_IF)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_IF))
        {
            stringList <<  FBBPositionToString(FBB_POSITION_IF);
        }
    }

    if (mask & FBB_POSITION_OF)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_OF))
        {
            stringList <<  FBBPositionToString(FBB_POSITION_OF);
        }
    }

    if (mask & FBB_POSITION_U)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_U))
        {
            stringList << FBBPositionToString(FBB_POSITION_U);
        }
    }

    if (mask & FBB_POSITION_P)
    {
        if (includeAggregates || !IsAgregatePosition(FBB_POSITION_P))
        {
            stringList << FBBPositionToString(FBB_POSITION_P);
        }
    }


    return stringList;
}

bool IsAgregatePosition(FBBPositionBits position)
{
    switch (position)
    {
        case FBB_POSITION_CI:
        case FBB_POSITION_MI:
        case FBB_POSITION_IF:
        case FBB_POSITION_U:
            return true;
        case FBB_POSITION_OF:
        case FBB_POSITION_P:
            return false;           // TODO: depends on settings
        default:
            return false;
    }
}