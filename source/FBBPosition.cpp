#include "FBB/FBBPosition.h"
#include "FBB/FBBLeaugeSettings.h"
#include "FBB/FBBApplication.h"

#include <QStringList>

QString FBBPositionMaskToString(FBBPositionMask mask, bool exclude)
{
    return FBBPositionMaskToStringList(mask, exclude).join("/");
}

QStringList FBBPositionMaskToStringList(FBBPositionMask mask, bool exclude)
{
    // String list we will build up
    QStringList stringList;

    // Only add positions we care about for this leauge
    const FBBLeaugeSettings::Positions positions = fbbApp->Settings()->positions;

    // Helper macro
#define HANDLE_POSITION(MASK, EXCLUDE, SETTING, POSITION)                   \
    {                                                                       \
        if (MASK & FBB_POSITION_##POSITION && SETTING.num##POSITION > 0) {  \
            stringList << #POSITION;                                        \
        }                                                                   \
    }

    // Hitting positions
    HANDLE_POSITION(mask, exclude, positions.hitting, C);
    HANDLE_POSITION(mask, exclude, positions.hitting, 1B);
    HANDLE_POSITION(mask, exclude, positions.hitting, 2B);
    HANDLE_POSITION(mask, exclude, positions.hitting, SS);
    HANDLE_POSITION(mask, exclude, positions.hitting, 3B);
    if (!exclude) {
        HANDLE_POSITION(mask, exclude, positions.hitting, MI);
        HANDLE_POSITION(mask, exclude, positions.hitting, CI);
    }
    HANDLE_POSITION(mask, exclude, positions.hitting, IF);
    HANDLE_POSITION(mask, exclude, positions.hitting, LF);
    HANDLE_POSITION(mask, exclude, positions.hitting, CF);
    HANDLE_POSITION(mask, exclude, positions.hitting, RF);
    HANDLE_POSITION(mask, exclude, positions.hitting, OF);
    HANDLE_POSITION(mask, exclude, positions.hitting, DH);

    // Pitching positions
    HANDLE_POSITION(mask, exclude, positions.pitching, SP);
    HANDLE_POSITION(mask, exclude, positions.pitching, RP);
    HANDLE_POSITION(mask, exclude, positions.pitching, P);

    if (!exclude || stringList.isEmpty()) {
        stringList << "U";
    }

    return stringList;
}