#include <assert.h>

#include "DraftSettings.h"

namespace Impl {

struct DraftSettingsImpl
{
    static DraftSettingsImpl& Get()
    {
        static DraftSettingsImpl s_impl;
        return s_impl;
    }

    DraftSettingsImpl()
    {
        OwnerCount = 12;
        HittingSplit = 0.70f;
        HitterCount = 14;
        PitcherCount = 10;
        Budget = 280;

        OwnerNames = QStringList{
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

        OwnerAbbreviations = QStringList{
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

        assert(OwnerNames.size() == OwnerCount);
        assert(OwnerAbbreviations.size() == OwnerCount);
    }

    QString UnknownString = "--";
    QString ErrorString = "??";

    uint32_t Budget;
    uint32_t HitterCount;
    uint32_t PitcherCount;
    float HittingSplit;
    uint32_t OwnerCount;
    QStringList OwnerNames;
    QStringList OwnerAbbreviations;
};

} // namespace Impl

using namespace Impl;

const uint32_t DraftSettings::Budget()
{
    return DraftSettingsImpl::Get().Budget;
}

const uint32_t DraftSettings::HitterCount()
{
    return DraftSettingsImpl::Get().HitterCount;
}

const uint32_t DraftSettings::PitcherCount()
{
    return DraftSettingsImpl::Get().PitcherCount;
}

const uint32_t DraftSettings::RosterSize()
{
    return HitterCount() + PitcherCount();
}

const float DraftSettings::HittingSplit()
{
    return DraftSettingsImpl::Get().HittingSplit;
}

const float DraftSettings::PitchingSplit()
{
    return 1.f - HittingSplit();
}

const uint32_t DraftSettings::OwnerCount()
{
    return DraftSettingsImpl::Get().OwnerCount;
}

const QString& DraftSettings::OwnerName(uint32_t i)
{
    if (i == 0) {
        return DraftSettingsImpl::Get().UnknownString;
    } else if (i > DraftSettingsImpl::Get().OwnerCount) {
        return DraftSettingsImpl::Get().ErrorString;
    }
    
    return DraftSettingsImpl::Get().OwnerNames[i-1];
}

const QStringList& DraftSettings::OwnerNames()
{
    return DraftSettingsImpl::Get().OwnerNames;
}

const QString& DraftSettings::OwnerAbbreviation(uint32_t i)
{
    if (i == 0) {
        return DraftSettingsImpl::Get().UnknownString;
    } else if (i > DraftSettingsImpl::Get().OwnerCount) {
        return DraftSettingsImpl::Get().ErrorString;
    }

    return DraftSettingsImpl::Get().OwnerAbbreviations[i-1];
}
