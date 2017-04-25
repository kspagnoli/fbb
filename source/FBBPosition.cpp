#include "FBB/FBBPosition.h"
#include "FBB/FBBLeaugeSettings.h"

#include <QStringList>

QString FBBPositionMaskToString(const FBBPositionMask& mask)
{
    // String list we will build up
    QStringList stringList;

    // Only add positions we care about for this leauge
    const FBBLeaugeSettings::Positions positions = FBBLeaugeSettings::Instance().positions;
     
    // Helper macro
#define HANDLE_POSITION(MASK, SETTING, POSITION)                        \
    if (MASK & FBB_POSITION_##POSITION && SETTING.num##POSITION > 0) {  \
        stringList << #POSITION;                                        \
    }

    // Hitting positions
    HANDLE_POSITION(mask, positions.hitting, C);
    HANDLE_POSITION(mask, positions.hitting, 1B);
    HANDLE_POSITION(mask, positions.hitting, 2B);
    HANDLE_POSITION(mask, positions.hitting, SS);
    HANDLE_POSITION(mask, positions.hitting, 3B);
    HANDLE_POSITION(mask, positions.hitting, MI);
    HANDLE_POSITION(mask, positions.hitting, CI);
    HANDLE_POSITION(mask, positions.hitting, IF);
    HANDLE_POSITION(mask, positions.hitting, LF);
    HANDLE_POSITION(mask, positions.hitting, CF);
    HANDLE_POSITION(mask, positions.hitting, RF);
    HANDLE_POSITION(mask, positions.hitting, OF);
    HANDLE_POSITION(mask, positions.hitting, DH);
    HANDLE_POSITION(mask, positions.hitting, U);

    // Pitching positions
    HANDLE_POSITION(mask, positions.pitching, SP);
    HANDLE_POSITION(mask, positions.pitching, RP);
    HANDLE_POSITION(mask, positions.pitching, P);
    
    // Return with delimiter
    return stringList.join("/");
}